#include <cassert> //macro assert() per evrificare le condizioni a runtime
#include <cstdint> //tipi di interi a dimmensione fissa come uint32_t
#include <iostream>
#include <iterator> //supporto agli iteratori STL
#include <string>
#include <thread>
#include <chrono>//usati nell'implementazione per animazione  o ritarrdi
#include<sstream>

/*Costante che vale 80, ovvero il valore a cui viene resettato il countdown del panico quando PicMan mangia un power
pellet(i fatasmi diventano vulnerabili per 80 tick)*/
constexpr uint32_t PANIC_RESET = 80;

/*La classe enum rappresenta il constenuto di ogni cella della griglia di gioco
È richiesto l'uso del prefisso cell_type:: affinchè enum class eviti collisioni di nomi*/
enum class cell_type {
    empty, //cella vuota
    wall,  //muro
    pellet, //pallina normale da mangiare
    power_pellet, //pallina potente che attiva il panico
    pacman, //poszione di picman
    ghost1, //posiziopne fantasma 1
    ghost2, //posiziopne fantasma 1
    ghost3, //posiziopne fantasma 1
    ghost4  //posiziopne fantasma 1
};

/*Eccezione personalizzata per il gioco. Il costruttore riceve un meddaggio di errore,
whta() ci restituisce il messaggio e infine viene lanciata l'eccezione con throw pacman_exception("messaggio")*/
struct pacman_exception {
    pacman_exception(std::string const& msg) : m_msg(msg) {}
    std::string const& what() const { return m_msg; }

private:
    std::string m_msg;
};

/*Rappresenta lo stato completo del gioco in un dato momento (un snapshot della partita)*/
struct game_state {
    game_state(); //costruttore di default
    game_state(uint32_t s); //costruttore con dimensione griglia sxs
    game_state(game_state const& rhs); //costruttore di copia
    game_state(game_state&& rhs);// move constructor
    game_state& operator=(game_state const& rhs); //copy assigment
    game_state& operator=(game_state&& rhs); //move assigment
    ~game_state();//distruttore - mi libera m_grind
    /*I copy/move constructo e assignment sono necessari perché game_state gestisce memoria
    dinamica (m_grind), seguendo le Rule of Five*/

    uint32_t get_size() const; //dimensione della griglia s x s
    uint32_t get_score() const; //punteggio attuale
    uint32_t get_lives() const; //vite rimaste
    uint32_t get_pellets_left() const; //palline ancora da mangiare
    uint32_t get_panic_countdown() const; //tick rimasti di panico
    bool win() const; //true se tutte le palline sono state mangiate

    void set_size(uint32_t s);
    void set_score(uint32_t score);
    void set_lives(uint32_t lives);
    void set_pellets_left(uint32_t pellets_left);
    void set_panic_countdown(uint32_t panic_countdown);

    //Operatore (),accesso alla griglia, permette di accedere alla cella (i, j), stile tipico x le matrici in Cpp. Alternativa a [][]
    cell_type& operator()(uint32_t i, uint32_t j); //lettura o scrittura
    cell_type operator()(uint32_t i, uint32_t j) const; //lettura

    /*Operatore di confronto, permee di confrontare due stati di gioco (griglia e tutti i campi)*/
    bool operator==(game_state const& rhs) const;
    bool operator!=(game_state const& rhs) const;

    /*Stampa la griglia come arte ASCII, # per i muri, . per le palline ecc*/
    void print_ascii_art(std::ostream& os) const;

private:
    uint32_t m_size; //dimensione n della griglia n x n
    uint32_t m_score; //punteggio
    uint32_t m_lives; //vita
    uint32_t m_pellets_left; //palline rimaste
    uint32_t m_panic_countdown; //countdown panico fantasmi
    cell_type** m_grid; /*matrice dinamica 2D di celle, m_grid è un array allocato dinamicamente*/

};

/*Operatori stream globali che permettono di scrivere o leggere uno stato su file o stream con << e >>*/
std::ostream& operator<<(std::ostream& os, game_state const& gs);//serializzazione
std::istream& operator>>(std::istream& is, game_state& gs);//deserializzazione

/*La classe pacman rappresenta l'intera stroia/cronologia della partita come una lista collegata semplice di game_state
ogni nodo rappresenta un passo della partita*/
class pacman {
    struct node {
        game_state gs; //lo stato del gioco in questo momento (griglia, punteggio, vite ecc)
        node* next; //puntatore al prossimo stato
    };

public:
    pacman();// crostruttore di default, creo uan  lisat vuota
    pacman(uint32_t size);//costruttore con dimensione, crea un game_state di dimensione size e lo inserisce come primo nodo della lista
    pacman(pacman const& rhs);//copy constructor
    pacman(pacman&& rhs);//move constructor, non copia ma 'ruba' i puntatori da rhs. ES. m_head = rhs.m_head, prende la testa.
    pacman& operator=(pacman const& rhs);
    pacman& operator=(pacman&& rhs);
    ~pacman();//libera tutti i nodi, uno ad uno, per evitare memory leak
    
    /*Metodi informativi: semplice getter sui campi privati, const perchè non modificano l'oggetto*/
    uint32_t size() const; //dimensione della griglia(es.10)
    uint32_t length() const; //nuemro di stati nella lista
    bool empty() const; //true se length() == 0

    /*Accesso agli stati, entrambi restituiscono un riferimento const, posos solo legegre e non modificare
    direttamente, per farlo devo usare move()*/
    game_state const& front() const;//primo stato (inizio partita). front() -> restituisce m_head->gs
    game_state const& back() const;//ultimo stato (stato attuale). back() -> restituisce m_tail->gs

    bool operator==(pacman const& rhs) const;
    bool operator!=(pacman const& rhs) const;

    struct iterator {
        using iterator_category = std::forward_iterator_tag;/*Iteratore in avanti: posso solo andare avanti da begin() a end()
        e non tronare indietro, ho una lisat collegata semplice (no prev per tornare indietro)*/
        using value_type = game_state;//il tipo che contiene
        using reference = game_state&;//quelo che restituisce operator*
        using pointer = game_state*;//quello che restituisce operatore ->

        iterator(node* ptr);/*Inizializza m_ptr = ptr. Qunaod chiamo begin(), ricevo un iteratore costruito da m_head
        Quando chiamo end(), ricevo un iteratore costruito con nullptr*/
        reference operator*();/*Deferenziazione: restituisce il game_state dentro il nodo corrente*/
        pointer operator->();/*Accesso ai membri: posso accedere direttamene ai metodi di game_state senza dereferenziare manualmente
        Esempio: auto it = mia_partita.begin();
                 it->get_score();   // equivale a (*it).get_score()*/
        iterator& operator++();/*Avanzamento: pre incremento ++it*/
        iterator operator++(int);/*Avanzamento: post incremento it++*/

        /*Operatori di confronto: confronta semplicemente i puntatori m_ptr*/
        bool operator==(iterator const& rhs) const;
        bool operator!=(iterator const& rhs) const;

    private:
        node* m_ptr;//Cuore dell'iteratore, m_ptr punatore al nodo corrente della lista
    };

    /*Questa struct è identica a iterator, ma restituisce riferimenti e puntatori const, percio non posso
    modificare gli stati attraverso di essa. Viene usato in automatico quando la lista è const*/
    struct const_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = game_state const;
        using reference = game_state const&;
        using pointer = game_state const*;

        const_iterator(node const* ptr);
        reference operator*() const;
        pointer operator->() const;
        const_iterator& operator++();
        const_iterator operator++(int);
        bool operator==(const_iterator const& rhs) const;
        bool operator!=(const_iterator const& rhs) const;

    private:
        node const* m_ptr; //ndoo const
    };

    /*Quando l'oggetto picman è non-const vengono chiamate le versioni che restituiscono iterator. 
    Quando è const vengono chiamate quelle che restituiscono const_iterator. Il cmpilatore sceglie in automatico*/
    iterator begin();//iterator (m_head)
    iterator end();//iterator(nullptr)
    const_iterator begin() const; //const_iterator(m_head)
    const_iterator end() const; //const_iterator(nullptr)
    
    void push_back(game_state const& gs);//Aggiunge un nodo in fondo alla lista
    
    /*Move - cuore dle gioco, questo metodo riceve:
    1. who: chi si muove (cell_type::pacman, cell_type::ghost1)
    2. delta_i: spostamento verticale (-1 su, + 1 giu, 0 fermo)
    3. delta_j: spostamento orizzontale (-1 sinistra, + 1 destra, 0 fermo)
    Ogni chiamata a move(), non sovrascrive lo stato precedente ma lo conserva aggiungendo un nuovo nodo. 
    Questo mi permette di avere la cronologia di una partita*/
    void move(cell_type who, int delta_i, int delta_j);

    void move(cell_type who); //facultative and not evaluated! with this you can implement a strategy (AI) that automatically plays the game

    /*Output: stampa l'ultimo stato (m_tail->gs) come arte ASCII*/
    void print_ascii_art(std::ostream& os) const;

private:
    uint32_t m_size;
    uint32_t m_length;
    node* m_head;
    node* m_tail;
};

/*Operatori stream: permettono di salvare e ricaricare una partita da file*/
std::ostream& operator<<(std::ostream& os, pacman const& g);//scrive ogni game_state in sequenza
std::istream& operator>>(std::istream& is, pacman& g);//legge i game_state ricostruendo la lista nodo dopo nodo


/*Copy constructor — copia profonda
cpppacman(pacman const& rhs);
Questo è il più delicato. Non puoi semplicemente copiare i puntatori, perché entrambi gli oggetti punterebbero agli stessi nodi in memoria — modificarne uno cambierebbe anche l'altro (dangling pointer disaster!).
Quindi bisogna fare una deep copy: scorrere tutta la lista di rhs e creare nodi nuovi uno per uno:
rhs:  [node A] → [node B] → [node C]
       ↓ copia ogni nodo
this: [node A'] → [node B'] → [node C']
*/

/* void move(cell_type who, int delta_i, int delta_j);
1. Copia l'ultimo stato (m_tail->gs) → new_state
2. Trova la posizione attuale di "who" nella griglia
3. Calcola la nuova posizione: (i + delta_i, j + delta_j)
4. Controlla la validità del movimento:
      - È dentro la griglia?
      - È un muro? → movimento bloccato (eccezione o ignorato)
5. Applica il movimento e gestisce le collisioni:
      - Pac-Man su pellet → score++, pellets_left--
      - Pac-Man su power_pellet → panic_countdown = PANIC_RESET (80)
      - Pac-Man su fantasma (senza panico) → lives--
      - Pac-Man su fantasma (con panico) → fantasma eliminato, score++
6. Aggiorna il panic_countdown (decrementa di 1 ad ogni mossa)
7. Chiama push_back(new_state) → aggiunge il nuovo stato alla lista*/

/*Spiegazione iteratori
Gli iteratori sono utili perchè ci permettono di scorrere la lista senza consocere i dettagli(nodi, puntatori)
*/

/*operator++ — avanzamento
Esistono due versioni:
cppiterator& operator++();     // pre-incremento:  ++it
iterator  operator++(int);  // post-incremento: it++
Pre-incremento (più efficiente):
cpp// implementazione concettuale:
m_ptr = m_ptr->next;  // sposta il puntatore al nodo successivo
return *this;         // restituisce se stesso già modificato
Post-incremento (meno efficiente):
cpp// implementazione concettuale:
iterator old = *this;  // salva una copia del vecchio stato
m_ptr = m_ptr->next;   // avanza
return old;            // restituisce la copia PRIMA dell'avanzamento
La differenza in pratica:
cppauto it = begin();

++it;  // it avanza, e puoi usarlo subito aggiornato
it++;  // it avanza, ma l'espressione vale il valore PRIMA dell'avanzamento
Per questo nelle iterazioni si preferisce sempre ++it.
*/