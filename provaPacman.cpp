#include "pacman.hpp"


//IMPLEMENTAZIONE GAME_STATE
game_state :: game_state(){
    m_size = 0;
    m_score = 0;
    m_lives = 0;
    m_pellets_left = 0;
    m_panic_countdown = 0;
    m_grid = nullptr;
}

game_state :: game_state(uint32_t s){
    m_size = 0;
    m_score = 0;
    m_lives = 0;
    m_pellets_left = 0;
    m_panic_countdown = 0;
    m_grid = nullptr;

    //Caso s == 0
    if(s == 0){
        //m_size = 0;
        return;
    }

    //Verifichiamo di avere potenze di 2 e almeno 4
    if(s < 4 || !( (s & (s - 1)) == 0 )) throw pacman_exception("Dimensione della griglia non valida");
    
    m_size = s;
    m_grid = new cell_type*[s];//creazione dell'array nel heap
    for(unsigned i = 0; i < s; i++){//Ciclo for esterno mi crea gli array di puntatori individualmente
        m_grid[i] = new cell_type[s];
        for(unsigned j = 0; j < s; ++j){
            m_grid[i][j] = cell_type::empty; //inizailizzo le celle dell'array a empty
        }
    }
}

//Copy constructor
game_state :: game_state(game_state const& rhs) :  m_size(rhs.get_size()), m_score(rhs.get_score()),
                                                    m_lives(rhs.get_lives()), m_pellets_left(rhs.get_pellets_left()),
                                                    m_panic_countdown(rhs.get_panic_countdown())
{
    //Se rhs.m_grid è nullptr -> alloca nullptr con il ternario, poi l'if fa return
    //Se rhs.m_grid non è nullptr -> alloca con il ternario, salta l'if e fa il ciclo
    m_grid = rhs.m_grid ? new cell_type*[rhs.m_size] : nullptr;
    
    if(rhs.m_grid == nullptr){
        m_grid = nullptr;
        return;
    }

    for(unsigned i = 0; i < m_size; i++){//Ciclo for esterno mi crea gli array di puntatori individualmente
        m_grid[i] = new cell_type[m_size];
        for(unsigned j = 0; j < m_size; ++j){
            m_grid[i][j] = rhs.m_grid[i][j]; //copio la cella da rhs
        }
    }
}


//Move constructor
game_state::game_state(game_state&& rhs) : m_size(rhs.get_size()), m_score(rhs.get_score()),
                                                    m_lives(rhs.get_lives()), m_pellets_left(rhs.get_pellets_left()),
                                                    m_panic_countdown(rhs.get_panic_countdown())
{
    m_grid = rhs.m_grid; //rubo il puntatore, non alloco nuova memoria, non creo nuove celle
    
    rhs.m_size = 0;
    rhs.m_score = 0;
    rhs.m_lives = 0;
    rhs.m_pellets_left = 0;
    rhs.m_panic_countdown = 0;
    rhs.m_grid = nullptr;
}

/*
game_state::game_state(game_state&& rhs) 
    : m_size(rhs.get_size()), 
      m_score(rhs.get_score()),
      m_lives(rhs.get_lives()), 
      m_pellets_left(rhs.get_pellets_left()),
      m_panic_countdown(rhs.get_panic_countdown())
L'initialization list copia i 5 scalari da rhs nel nuovo oggetto. 
m_grid = rhs.m_grid; //^rubo il puntatore, non alloco nuova memori, non creo nuove celle.
Il nuovo oggetto ora punta alla stessa griglia che aveva rhs
Azzera rhs — questo è fondamentale. Dopo il move, rhs non deve più "possedere" la griglia. 
Se non facessi rhs.m_grid = nullptr, quando il distruttore di rhs viene chiamato farebbe 
delete[] sulla memoria che ora appartiene al nuovo oggetto → crash.
prima:   rhs.m_grid ──→ [griglia]
dopo:    this.m_grid ──→ [griglia]    rhs.m_grid ──→ nullptr
Una sola griglia in memoria, zero allocazioni — molto più veloce.
Il compilatore lo usa automaticamente quando l'oggetto sorgente è temporaneo e sta per essere distrutto, ad esempio:

game_state crea_stato() {
    game_state gs(8);
    return gs; // ← qui il compilatore usa il move, non il copy
}*/


//Copy assignment: viene liberata prima la griglia corrente
game_state& game_state::operator=(game_state const& rhs){

    if(this == &rhs) return *this; //*this esisite già e dobbiamo liberarla perchè potrebbe avere una griglia allocata. sef-assigemnet(gs = gs), non succede nulla
    
    //Liberare la griglia attuale, inizio eliminando ogni riga della griglia e poi elimino m_grind 
    if(m_grid != nullptr){
        for(unsigned i = 0; i < m_size; i++){
                delete [] m_grid[i]; //inizio eliminando gli elementi indivualmenete
        }
        delete [] m_grid;//elimino l'array
    }
    //Copio i valori da rhs in *this, negli operatori di assegnamento devo assegnare gli scalari dentro il corpo con =
    m_size = rhs.get_size();
    m_score = rhs.get_score();
    m_lives = rhs.get_lives();
    m_pellets_left = rhs.get_pellets_left();
    m_panic_countdown = rhs.get_panic_countdown();

    
    if(rhs.m_grid == nullptr){
        m_grid = nullptr;
        return *this;
    }

    //Prima del ciclo for, devo allocare la griglia
    m_grid = new cell_type*[m_size];
    for(unsigned i = 0; i < m_size; i++){
        m_grid[i] = new cell_type[m_size];
        for(unsigned j = 0; j < m_size; ++j){
            m_grid[i][j] = rhs.m_grid[i][j]; //copio la cella da rhs
        }
    }

    return *this;
}

/*
1. Self-assignment check
if(this == &rhs) return *this;
this è il puntatore all'oggetto a sinistra dell'=, &rhs è l'indirizzo dell'oggetto
a destra. Se sono lo stesso oggetto (gs = gs) non fare nulla e ritorna subito — 
altrimenti libereresti la memoria che stai cercando di copiare.
2. Libera la griglia attuale
for(unsigned i = 0; i < m_size; i++){
    delete [] m_grid[i];
}
delete [] m_grid;
*this esiste già e potrebbe avere una griglia allocata. Prima di sovrascriverla 
devi liberarla, altrimenti quella memoria va persa per sempre (memory leak). 
Prima elimini ogni riga, poi l'array di puntatori — ordine inverso rispetto all'allocazione.
3. Copia i 5 scalari
m_size = rhs.get_size();
m_score = rhs.get_score();
Semplice copia di valori numerici da rhs in *this

4. Caso griglia nulla
cppif(rhs.m_grid == nullptr){
    m_grid = nullptr;
    return *this;
}
Se rhs è uno stato vuoto (size=0), non c'è nulla da copiare. 
Assegna nullptr e ritorna.

5. Alloca e copia la griglia
cppm_grid = new cell_type*[m_size];
for(unsigned i = 0; i < m_size; i++){
    m_grid[i] = new cell_type[m_size];
    for(unsigned j = 0; j < m_size; ++j){
        m_grid[i][j] = rhs.m_grid[i][j];
    }
}
Alloca una nuova griglia indipendente e copia cella per cella. 
Dopo questa operazione *this e rhs hanno griglie separate in memoria
Gli operatori di assegnamento ritornano sempre *this per permettere catene tipo a = b = c
*/

//Move assigment
/*game_state& game_state::operator=(game_state&& rhs){

    if(this == &rhs){
        return *this;
    }

    //Liberare la griglia attuale, inizio eliminando ogni riga della griglia e poi elimino m_grind 
    if(m_grid != nullptr){
        for(unsigned i = 0; i < m_size; i++){
                delete [] m_grid[i]; //inizio eliminando gli elementi indivualmenete
        }
        delete [] m_grid;//elimino l'array
    }

    m_size = rhs.get_size();
    m_score = rhs.get_score();
    m_lives = rhs.get_lives();
    m_pellets_left = rhs.get_pellets_left();
    m_panic_countdown = rhs.get_panic_countdown();
    m_grid = rhs.m_grid;

    rhs.set_score(0);
    rhs.set_lives(0);
    rhs.set_pellets_left(0);
    rhs.set_panic_countdown(0);

    rhs.m_size = 0;
    rhs.m_grid = nullptr;

    return *this;
}*/

//Move assignment senza getter e setter
game_state& game_state::operator=(game_state&& rhs){

    if(this == &rhs){
        return *this;
    }

    if(m_grid != nullptr){
        for(unsigned i = 0; i < m_size; i++){
            delete[] m_grid[i];
        }
        delete[] m_grid;
    }

    m_size = rhs.m_size;
    m_score = rhs.m_score;
    m_lives = rhs.m_lives;
    m_pellets_left = rhs.m_pellets_left;
    m_panic_countdown = rhs.m_panic_countdown;
    m_grid = rhs.m_grid;

    rhs.m_size = 0;
    rhs.m_score = 0;
    rhs.m_lives = 0;
    rhs.m_pellets_left = 0;
    rhs.m_panic_countdown = 0;
    rhs.m_grid = nullptr;

    return *this;
}


//Distruttore
game_state ::~game_state(){

    if(m_grid != nullptr){
        for(unsigned i = 0; i < m_size; i++){
            delete[] m_grid[i];
        }
    }
    delete[] m_grid;
}

//Getters
uint32_t game_state::get_size() const {
    return m_size;
}

uint32_t game_state::get_score() const {
    return m_score;
}

uint32_t game_state::get_lives() const {
    return m_lives;
}

uint32_t game_state::get_pellets_left() const {
    return m_pellets_left;
}

uint32_t game_state::get_panic_countdown() const {
    return m_panic_countdown;
}

bool game_state::win() const{
    if(m_pellets_left != 0) return false;
    else return true;
    //oppure m_pallets_left == 0;
}


//set_size puo essere chiamato solo se m_grid == nullptr, ritorna mm_grind[i][j] (riferimento nella versione non-const)
void game_state::set_size(uint32_t s){
    
    if(m_grid != nullptr) throw pacman_exception("La griglia è già allocata");

    if(s == 0){
        m_size = 0;
        return;
    }
    
    if(s < 4 || !((s & (s - 1)) == 0 )) {
        throw pacman_exception("Dimensione della griglia non valida");
    }

    m_size = s;
    m_grid = new cell_type*[s];
    for(unsigned i = 0; i < s; i++){
        m_grid[i] = new cell_type[s];
        for(unsigned j = 0; j < s; ++j){
            m_grid[i][j] = cell_type::empty; 
        }
    }
}


void game_state::set_score(uint32_t score){
    m_score = score;
}

void game_state::set_lives(uint32_t lives){
    m_lives = lives;
}

void game_state::set_pellets_left(uint32_t pellets_left){
    m_pellets_left = pellets_left;
}

void game_state::set_panic_countdown(uint32_t panic_countdown){
    m_panic_countdown = panic_countdown;
}

//Operatore ()
//Restituisco un riferimento
cell_type& game_state::operator()(uint32_t i, uint32_t j){

    if(i >= m_size || j >= m_size){
        throw pacman_exception("Gli indici sono fuori dai limiti della griglia");
    } else {
        return m_grid[i][j];
    }
}

//Restituisco un valore perchè l'oggeto è const
cell_type game_state::operator()(uint32_t i, uint32_t j) const{

    if(i >= m_size || j >= m_size){
        throw pacman_exception("Gli indici sono fuori dai limiti della griglia");
    } else {
        return m_grid[i][j];
    }
}


//Confrontare il contenuto della griglia
bool game_state::operator==(game_state const& rhs) const{

    if(m_size != rhs.m_size || m_score != rhs.m_score || m_lives != rhs.m_lives 
       || m_pellets_left != rhs.m_pellets_left || m_panic_countdown != rhs.m_panic_countdown){
            return false;
    }

    //if(m_grid == nullptr || rhs.m_grid == nullptr){ return m_grid == rhs.m_grid;}

    if (m_grid == nullptr && rhs.m_grid == nullptr) return true;

    for(unsigned i = 0; i < m_size; i++){
        for(unsigned j = 0; j < m_size; ++j){
            if(m_grid[i][j]  != rhs.m_grid[i][j]){
                return false;
            } 
        }
    }
    return true;
}

bool game_state::operator!=(game_state const& rhs) const{
    return !(*this == rhs);
}
/*
*this == rhs
Questa espressione chiama  l'operator==:
(*this).operator==(rhs) che è equivalente a: a == b*/


void game_state::print_ascii_art(std::ostream& os) const {

    os << "score=" << m_score
       << "  lives=" << m_lives
       << "  pellets_left=" << m_pellets_left
       << "  panic_countdown=" << m_panic_countdown
       << "  size=" << m_size
       << '\n';

    os << '+';
    for(uint32_t i = 0; i < m_size; ++i){
        os << '-';
    }
    os << "+\n";

    for(uint32_t i = 0; i < m_size; ++i){

        os << '|';

        for(uint32_t j = 0; j < m_size; ++j){

            switch((*this)(i,j)){

                case cell_type::empty:
                    os << '.';
                    break;

                case cell_type::wall:
                {
                    bool horizontal =
                        (j > 0 && m_grid[i][j-1] == cell_type::wall) ||
                        (j + 1 < m_size && m_grid[i][j+1] == cell_type::wall);

                    bool vertical =
                        (i > 0 && m_grid[i-1][j] == cell_type::wall) ||
                        (i + 1 < m_size && m_grid[i+1][j] == cell_type::wall);

                    if(horizontal)
                        os << '-';
                    else if(vertical)
                        os << '|';
                    else
                        os << '-';

                    break;
                }

                case cell_type::pellet:
                    os << '.';
                    break;

                case cell_type::power_pellet:
                    os << 'O';
                    break;

                case cell_type::pacman:
                    os << 'x';
                    break;

                case cell_type::ghost1:
                    os << '1';
                    break;

                case cell_type::ghost2:
                    os << '2';
                    break;

                case cell_type::ghost3:
                    os << '3';
                    break;

                case cell_type::ghost4:
                    os << '4';
                    break;
            }
        }

        os << "|\n";
    }

    os << '+';
    for(uint32_t i = 0; i < m_size; ++i){
        os << '-';
    }
    os << "+\n";

} 
/*--------------------------------Fine implementazione game_state-----------------------------------*/

//IMPLEMENTAZIONE DI PACMAN
 
//Costruttore: contenitore vuoto
pacman ::  pacman(){
    m_size  = 0;
    m_length = 0;
    m_head = nullptr;
    m_tail = nullptr;
}

//Costruttore con parametro - creare un contenitore vuoto. Controllo la correttezza del size: se è uguale 0, a una potenza di due
//o a 4 -> OK. Altrimenti, lancio un'eccezione.
/*
  Recap funzione: inizializzo l'oggetto. Se la size passata come parametro è 0 -> contenitore vuoto -> fine.
  Se la size  è diversa da zero, allora controllo la validità (ovvero se è una potenza di 2 maggiore o uguale a 4),
  se la size non è valida lancio un'eccezione. Se size è valida assegno sizea m_size.
*/
pacman :: pacman(uint32_t size){
   
    m_size  = 0; //dimensione massima della lista
    m_length = 0; //il numero dei nodi presenti nella lista
    m_head = nullptr;
    m_tail = nullptr;

    //Caso size == 0 allora il contenitore è vuoto
    if(size == 0){
        return;
    }

    //Verifichiamo di avere potenze di 2 e almeno 4
    if(size < 4 || !( (size & (size - 1)) == 0 )){
        throw pacman_exception("Dimensione della griglia non valida");
    }

    //Game-size pre-set: ricevo come parametro size(es. 16), quindi la dimensione del gioco sarà size (16), percio m_size deve essere uguale a size e non a 0
    m_size  = size; //dimensione massima della lista
}


//Costruttore di copia. Leggo da rhs (invece che da cin)
/*Costruire il nuovo oggetto (this); rhs è l'oggetto da cui copio i dati, esiste già e non va modificato*/
pacman :: pacman(pacman const& rhs) {
     
    //Copio la size e length, dato che sono variabili normali(int ecc) posso copiarli direttamente
    m_size = rhs.m_size;
    m_length = rhs.m_length;
  
    //Inizialmente la nuova lista è vuota, sto costruendo un nuovo oggetto
    m_head = m_tail = nullptr;
    
    //Creo un puntatore per scorrere la lista. Scorro la lista per arrivare alla fine
    node* p = rhs.m_head;
    while(p != nullptr){
        //Per ogni nodo di rhs -> creo un nuovo nodo
        node* nuovo = new node();
        nuovo -> gs = p -> gs;
        nuovo -> next = nullptr;

        //Se la lista è vuota, inserimento in testa. Altrimenti, inserimento in coda
        if(m_head == nullptr){
            m_head = m_tail = nuovo;
        } else { 
            m_tail -> next = nuovo;
            m_tail = nuovo;
        }
        p = p -> next;
    }
}

//Costruttore move (l'oggetto esiste già): qui non devo creare nuovi nodi (a differenza del costruttore di copia), ma rubo i dati e svuoto il vecchio oggetto
pacman :: pacman(pacman&& rhs){

    m_size = rhs.m_size;
    m_length = rhs.m_length;
    m_head = rhs.m_head;
    m_tail = rhs.m_tail;

    rhs.m_size = 0;
    rhs.m_length = 0;
    rhs.m_head = nullptr;
    rhs.m_tail = nullptr;
}

//Distruttore
pacman :: ~ pacman(){
    node* corrente = m_head;
    while (corrente != nullptr){
        node* elimina = corrente;
        corrente = corrente ->next;
        delete elimina;
    }
    
    m_head = m_tail = nullptr;
}

//Copy assignment
//Nota: assomiglia al costruttore di copia solo che qui l'oggetto esiste già, quindi prima devo pulire la lista vecchia e poi copiare
pacman& pacman::operator=(pacman const& rhs){
    
    //Self assignment, se sto copiando me stesso faccio return subito
    if(this == &rhs) return *this;

    //Se non si verifica self-assignment, procedo eliminando la vecchia lista
    node* corrente = m_head;
    while (corrente != nullptr){
        node* elimina = corrente;
        corrente = corrente ->next;
        delete elimina;
    }

    /*Dopo aver cancellato tutti i nodi, m_head contiene ancora l'indirizzo ldel primo nodo
    ma esso non esiste piu, è corretto inizzalizzare tutto a nullptr*/
    m_head = m_tail = nullptr;

    //Copio i dati semplici (ovvero m_size e m_length)
    m_size = rhs.m_size;
    m_length = rhs.m_length;

    //Ricostruzione di una nuova lista (uguale a come ho fatto nel costruttore di copia)
    //Creo un puntatore per scorrere la lista. Scorro la lista per arrivare alla fine
    node* p = rhs.m_head;
    while(p != nullptr){
        //Per ogni nodo di rhs -> creo un nuovo nodo
        node* nuovo = new node();
        nuovo -> gs = p -> gs;
        nuovo -> next = nullptr;

        //Se la lista è vuota, inserimento in testa. Altrimenti, inserimento in coda
        if(m_head == nullptr){
            m_head = m_tail = nuovo;
        } else { 
            m_tail -> next = nuovo;
            m_tail = nuovo;
        }
        p = p -> next;
    }

    return *this;
}


//Move assignment. Frees the current history and steals rhs's.
pacman& pacman::operator=(pacman&& rhs){

    if(this == &rhs){
        return *this;
    }

    node* corrente = m_head;
    while (corrente != nullptr){
        node* elimina = corrente;
        corrente = corrente ->next;
        delete elimina;
    }

    //Azzero i puntatori
    m_head = m_tail = nullptr;

    //Rubo i dati, divento 'io' il proprietario dell'oggetto
    m_size = rhs.m_size;
    m_length = rhs.m_length;
    m_head = rhs.m_head;
    m_tail = rhs.m_tail;

    //Svuoto rhs, diventa oggetto vuoto. 
    rhs.m_size = 0;
    rhs.m_length = 0;
    rhs.m_head = nullptr;
    rhs.m_tail = nullptr;

    return *this;
}

//Getters
uint32_t pacman:: size() const{
    return m_size;
}

uint32_t pacman:: length() const{
    return m_length;
}

bool pacman:: empty()  const{
    return m_length == 0;
}

//Primo snapshot della cronologia
game_state const& pacman::front() const{

    if(m_length == 0){
        throw pacman_exception("Il contenitore è vuoto");
    }
    return m_head->gs;
}  


//L'ultimo snapshot della cronologia, all'interno del nodo ho il game state
game_state const& pacman::back() const{

    if(m_length == 0){
        throw pacman_exception("Il contenitore è vuoto");
    }
    return m_tail->gs;
}  

/*
Due oggetto pacman rappresentano la stessa cronologia
Confronto due liste e restituisco true sono nel caso di size uguale, length uguale e tutti game_state uguali
*/

bool pacman :: operator==(pacman const& rhs) const{

    if(m_size != rhs.m_size || m_length != rhs.m_length){
        return false;
    }

    //Per rendere il codice piu coerente con la firma della fuzione, uso const,
    //visto che semplicemente leggo il dei nodi delle liste.
    //l1 e l2 sono spuntatori per scorere la lista 
    node const* l1 = m_head;
    node const* l2 = rhs.m_head;

    /*Posso usare l'operatore '!=' perche l1->gs e l2->gs sono game_state e quindi quando compilo,
    il compilatore cera la funzione game_state::operator!=(game_state const&)*/
    while(l1 != nullptr && l2 != nullptr){
        if(l1->gs != l2->gs){
            return false;
        }
        l1 = l1->next;
        l2 = l2->next;
    }

    //Verifico se i due puntatori abbiano raggiunto la fine della lista
    if(l1 != nullptr || l2 != nullptr){
        return false;
    }

    return true;
}

/*Con *this == rhs -> chiamo l'operatore==. Accade che (*this).operatore==(rhs): restituisce
true se i due pacman sono uguali e false se sono diversi. con coperatore ! inverto il risultato:$
se == mi restituisce true  allora ! è false -> l'operatore != significa che i due oggetti sono uguale, 
mentre se == mi restituisce false, ! è true -> l'operatore!= significa che i due oggetti sono diversi*/
bool pacman :: operator!=(pacman const& rhs) const{
    return !(*this == rhs);
}

/*--------------------------------------Fine implementazione Pacman---------------------------------------------*/

//IMPLEMENTAZIONE DEGLI ITERATORI
pacman :: iterator :: iterator(node* ptr){
    m_ptr = ptr;
}

pacman :: iterator :: reference pacman ::  iterator ::  operator*(){

   if(m_ptr == nullptr) throw pacman_exception{"Deferenzziazione dell'iteratore nullo"};
    return m_ptr -> gs; 

}

//L'operatore -> mi restituisce il puntatore all'elemento a cui l'iteratore fa riferimento, ovvero al gs (game state)
pacman :: iterator :: pointer pacman:: iterator :: operator->(){

    if(m_ptr == nullptr) throw pacman_exception{"Punta all'iteratore nullo"};
    return &(m_ptr -> gs); //qui mi viene restituito l'indirizzo di memoria  dell'oggetto gs contenuto nel nodo
}

//++i
pacman :: iterator& pacman :: iterator :: operator++(){

    if(m_ptr != nullptr) m_ptr = m_ptr -> next;
    return *this; //retituisco me stesso

}

//i++
pacman :: iterator pacman :: iterator :: operator++(int){

    iterator temp = *this;//creo iteratore temp. perchè devo restituire un iteratore
    ++(*this);
    return temp; //restituisco m_ptr prima dell'incremento
}

bool  pacman :: iterator:: operator==(iterator const& rhs) const{

    if(m_ptr != rhs.m_ptr) return false;
    return true;
}

bool  pacman :: iterator:: operator!=(iterator const& rhs) const{
    return !(m_ptr == rhs.m_ptr);
}


//IMPLEMENTAZIONE ITERATORI CONST

pacman :: const_iterator :: const_iterator(node const* ptr){
    m_ptr = ptr;
}

pacman :: const_iterator :: reference pacman ::  const_iterator ::  operator*() const{

    if(m_ptr == nullptr) throw pacman_exception{"Tentativo di deferenziare un iteratore nullo"};
    return m_ptr -> gs; 
}

pacman :: const_iterator :: pointer pacman:: const_iterator :: operator->() const{

    if(m_ptr == nullptr) throw pacman_exception{"Tentativo di accedere a un gs tarmite un iteratore nullo"};
    return &(m_ptr -> gs);
}

//++i
pacman :: const_iterator& pacman :: const_iterator :: operator++(){

    if(m_ptr != nullptr) m_ptr = m_ptr -> next;
    return *this; //retituisco me stesso
}

//i++
pacman :: const_iterator pacman :: const_iterator :: operator++(int){

    const_iterator temp = *this;//creo iteratore temp. perchè devo restituire un iteratore
    ++(*this);
    return temp; //restituisco m_ptr prima dell'incremento
}

bool  pacman :: const_iterator:: operator==(const_iterator const& rhs) const{

    if(m_ptr != rhs.m_ptr) return false;
    return true;
}

bool  pacman :: const_iterator:: operator!=(const_iterator const& rhs) const{
    return !(m_ptr == rhs.m_ptr);
}


//Implementazione di iterator begin();  e iterator end();

//Restituisco un iteratore che punta all'inizio della lista pacman
pacman :: iterator pacman ::  begin(){

    iterator it = iterator(m_head);
    return it;
}

//Restitusco un iteratore che punta all'alla fine della lista pacman (fine = coda)
pacman :: iterator pacman :: end(){

    //iterator it = iterator(m_tail);
    //return it;
    return iterator(nullptr);
}

//Implementazione di const_iterator begin() const; e const_iterator end() const;
pacman :: const_iterator pacman ::  begin() const{

    const_iterator it = const_iterator(m_head);
    return it;
}

pacman :: const_iterator pacman :: end() const{

    //const_iterator it = const_iterator(m_tail);
    //return it;
    return const_iterator(nullptr);
}


//----------------------------------------Fine implementazione degli iteratori------------------------------------------

void pacman :: push_back(game_state const& gs){

    uint32_t s = gs.get_size();//ho dimensione della griglia 

    if(m_length != 0){
        if(s != m_size){
            throw pacman_exception{"Dimensione della griglia diversa da pacman"};
        }
    }

    if(s > 0){
        if(gs(0,0) == cell_type :: wall || gs(s - 1, 0) == cell_type :: wall ||  gs(0, s -1) == cell_type :: wall ||  gs(s - 1 , s-1) == cell_type :: wall ||  gs(s/2, s/2) == cell_type :: wall){
            throw pacman_exception {"Wall trovato"}; 
        }
    }
    

    node* new_node = new node {gs, nullptr};

    if(m_head == nullptr){
       m_head = m_tail = new_node;
       m_length++;
    } else {
        m_tail -> next = new_node;
        new_node -> next = nullptr;
        m_tail = new_node;
        m_length++;
    }


}


void pacman :: move(cell_type who, int delta_i, int delta_j){

    /*Invalid move(...) arguments. move() throws pacman_exception if the history is empty, if who is not one of pacman, ghost1, ghost2, ghost3, ghost4, 
    if (delta_i, delta_j) is not a unit cardinal step, or if who is not present on the grid of the last state.*/
    
    //Cronologia vuota: lancio eccezione
    if(m_length ==  0){
        throw pacman_exception{"La coronologia è vuota"};
    }

    //Capire se è necessario?
    if(who != cell_type::pacman && who != cell_type::ghost1 && who != cell_type::ghost2 && who != cell_type::ghost3 && who != cell_type::ghost4){
        throw pacman_exception{"Who non è uno dei seguenti personaggio: pacman, ghost1, ghost2, ghost3, ghost4"};
    }

    //Controllo se i valori di delta_i e delta_j sono validi, le mosse consentite sono (-1,0) || (1,0) || (0,-1) || (0,1)
    if(!((delta_i == -1 && delta_j == 0) || (delta_i == 1  && delta_j == 0) || (delta_i == 0  && delta_j == -1) || (delta_i == 0  && delta_j == 1))) {
        throw pacman_exception{"La direzione non è valida"}; //es. se mi passano (1,1) o (2,0) o (0,0)
    }

    //Prendo l'ultimo game_state e creo una copia di esso, modifico la copia e infine la inserisco nella croologia con push_back()
    game_state nuovo_stato = back();
    
    //Variabili che utilizo in vari punti della funzione
    uint32_t pallets_left = nuovo_stato.get_pellets_left();
    uint32_t score = nuovo_stato.get_score();
    uint32_t  panic_mode_timer = nuovo_stato.get_panic_countdown();
    //Ad ogni move decremento il panic_countdown
    if(panic_mode_timer > 0){//
        panic_mode_timer--;
        nuovo_stato.set_panic_countdown(panic_mode_timer);
    }
    uint32_t lives_pacman = nuovo_stato.get_lives();

    /*Verifico il personaggio who, se non è uno dei pacman, ghost1, ghost2, ghost3, ghost4 -> lancio un eccezione.
      Inoltre, salvo la posizione del personaggio. */
    
    bool  personaggio = false;
    uint32_t riga = 0;
    uint32_t colonna = 0;

    for(uint32_t i = 0; i < nuovo_stato.get_size() && !personaggio; i++){
        for(uint32_t j = 0; j < nuovo_stato.get_size() && !personaggio; j++){
            if(nuovo_stato(i, j) == who){
                riga = i;
                colonna = j;
                personaggio = true;
            }
        }
    }
    
    if (!personaggio) {
        throw pacman_exception{"Personaggio non presente nella griglia"};
    }

    //---------------------------------Fine invalid move()-------------------------------------------------------------------------------------------

    /*Direction. Exactly one of delta_i, delta_j must be non-zero, and equal to +1 or -1. The four legal pairs are (-1, 0) (up), (+1, 0) (down), (0, -1) (left), (0, +1) (right).
      Walls / out of bounds. If a character attempts to move into a wall, or off the edge of the grid, its position stays unchanged in the new state. 
      A new state is still appended to the history anyway
    */

    //Calcolo la destinazione dopo la mossa, ovvero determino la nuova posizione di who
    int nuova_riga  = int(riga) + delta_i;
    int nuova_colonna = int(colonna) + delta_j;

    int size_griglia = int(nuovo_stato.get_size());//ricavo m_size

    //La destinazione deve essere diversa dalla posizione prima della mossa 
    if(nuova_riga < 0 || nuova_riga >= size_griglia || nuova_colonna < 0 || nuova_colonna >= size_griglia){
        push_back(nuovo_stato);
        return;
    }

    /*Prima di spostarmi, verifico il contenuto della destionazione, se ho un wall, il personaggio non si muove,
      ma aggiungo lo stesso un nuovo stato*/
    cell_type contenuto_cella = nuovo_stato(nuova_riga, nuova_colonna);

    if(contenuto_cella ==  cell_type :: wall){
        push_back(nuovo_stato);
        return;
    }

    /*Ghost starting / reset positions. The four ghost corners are ghost1 = (0, 0), ghost2 = (0, size-1), 
    ghost3 = (size-1, 0), ghost4 = (size-1, size-1). A valid game must contain no wall in any of those 
    four cells. Likewise, the centre cell (size/2, size/2) must not contain a wall. (Characters may occupy 
    it -- for instance, Pac-Man respawns there on death -- but the game layout never places a wall on it.) 
    pacman::push_back enforces this on every state appended to the history
    */
  
    if(nuovo_stato(0,0) == cell_type::wall || nuovo_stato(0, size_griglia-1) == cell_type::wall  || nuovo_stato(size_griglia-1, 0) == cell_type::wall 
        || nuovo_stato( size_griglia-1, size_griglia-1) == cell_type::wall || nuovo_stato( size_griglia/2, size_griglia/2) == cell_type::wall){
            throw   pacman_exception{"La cella contiene un muro"};
    }


    /*Two ghosts. Two ghosts cannot occupy the same cell. If a ghost attempts to move into another ghost's cell, 
      its position stays unchanged (they are not swapped).*/

    if(who  == cell_type::ghost1 || who  == cell_type::ghost2 || who  == cell_type::ghost3 || who  == cell_type::ghost4){
        if(contenuto_cella == cell_type::ghost1 || contenuto_cella == cell_type::ghost2 || contenuto_cella == cell_type::ghost3 || contenuto_cella == cell_type::ghost4){
            push_back(nuovo_stato);
            return;
        }
    }

    /*Ghost into a pellet. A cell can clearly contain only one value of cell_type, hence when a ghost tries to step into a neighbouring cell containing a (power) pellet, 
    the two are swapped: the ghost ends up on the old (power) pellet cell, and the (power) pellet ends up on the ghost's old empty cell. pellets_left is unchanged.*/
    if(who  == cell_type::ghost1 || who  == cell_type::ghost2 || who  == cell_type::ghost3 || who  == cell_type::ghost4){
        if(contenuto_cella == cell_type::pellet){
            nuovo_stato(riga, colonna) = cell_type::pellet;
            nuovo_stato(nuova_riga, nuova_colonna) = who;
            push_back(nuovo_stato);
            return;
        }else if(contenuto_cella == cell_type::power_pellet){
            nuovo_stato(riga, colonna) = cell_type::power_pellet;
            nuovo_stato(nuova_riga, nuova_colonna) = who;
            push_back(nuovo_stato);
            return;
        }
    }
    
    //Pac-Man / ghost contact.
    bool ghost_trovato = false;
    uint32_t riga_g = 0; //riga ghost
    uint32_t colonna_g = 0; //colonna ghost

    /*Pac-Man / ghost contact. If a move would cause Pac-Man and a ghost to share a cell -- whether Pac-Man moved into the ghost or the ghost moved into Pac-Man -- the outcome depends on the mode:*/
    if(who == cell_type:: pacman){

        if(contenuto_cella == cell_type::ghost1 || contenuto_cella == cell_type::ghost2 || contenuto_cella == cell_type::ghost3 || contenuto_cella == cell_type::ghost4){
            if(panic_mode_timer <= 0){//aggiungere un assert caso < 0
                if(lives_pacman > 0){
                    lives_pacman--;
                }
                nuovo_stato.set_lives(lives_pacman);
                nuovo_stato(riga, colonna) = cell_type::empty;//setto la poszione vecchia a empty
                //Ottengo le coordinate del centro, es. griglia 16x16: 16/2 = 8, il centro è (8, 8)
                int riga_centro = size_griglia/2; 
                int colonna_centro = size_griglia/2;
                cell_type  centro = nuovo_stato(riga_centro, colonna_centro);

                //Ricerca dei ghost -> riposizionamento
                /*Se un fantasma è stato mangiato durante una panic mode, non sarà più presente nella griglia.
                Quindi: il ciclo non lo trova; ghost_trovato rimane false; il blocco if(ghost_trovato) non viene eseguito; il fantasma non viene rimesso nell'angolo.*/
                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost1){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(0,0) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(0,0) = cell_type::ghost1;
                    } else if(nuovo_stato(0,0) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(0, 0) = cell_type::ghost1;
                    } else if(nuovo_stato(0,0) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(0, 0) = cell_type::ghost1;
                    }else{//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(0,0);
                        nuovo_stato(0,0) = cell_type::ghost1;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;

                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost2){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(0,size_griglia-1) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(0,size_griglia-1) = cell_type::ghost2;
                    } else if(nuovo_stato(0,size_griglia-1) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(0, size_griglia-1) = cell_type::ghost2;
                    } else if(nuovo_stato(0,size_griglia-1) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(0, size_griglia-1) = cell_type::ghost2;
                    } else {//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(0,size_griglia-1);
                        nuovo_stato(0,size_griglia-1) = cell_type::ghost2;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;
        
                //Risposiziono il ghost
                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost3){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(size_griglia-1,0) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(size_griglia-1,0) = cell_type::ghost3;
                    } else if(nuovo_stato(size_griglia-1,0) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(size_griglia-1, 0) = cell_type::ghost3;
                    } else if(nuovo_stato(size_griglia-1,0) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(size_griglia-1, 0) = cell_type::ghost3;
                    } else {//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(size_griglia-1, 0);
                        nuovo_stato(size_griglia-1, 0) = cell_type::ghost3;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;
            

                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost4){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(size_griglia-1,size_griglia-1) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(size_griglia-1,size_griglia-1) = cell_type::ghost4;
                    } else if(nuovo_stato(size_griglia-1,size_griglia-1) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(size_griglia-1,size_griglia-1) = cell_type::ghost4;
                    } else if(nuovo_stato(size_griglia-1,size_griglia-1) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(size_griglia-1,size_griglia-1) = cell_type::ghost4;
                    } else {//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(size_griglia-1, size_griglia-1);
                        nuovo_stato(size_griglia-1, size_griglia-1) = cell_type::ghost4;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;
                

                //Controllo contenuto della cella in centro
                if(centro == cell_type::empty){
                }else if(centro == cell_type::pellet){
                    pallets_left--;
                    nuovo_stato.set_pellets_left(pallets_left);
                    score += 10;
                    nuovo_stato.set_score(score);
                }else if(centro == cell_type::power_pellet){
                    pallets_left--;
                    nuovo_stato.set_pellets_left(pallets_left);
                    score += 50;
                    nuovo_stato.set_score(score);
                    panic_mode_timer = PANIC_RESET;
                    nuovo_stato.set_panic_countdown(panic_mode_timer);
                }//caso limite ghost in centro, riposizionare nuovo_stato(riga_centro, colonna_centro) = cell_type::pacman;fuori if, prima riposizioni i ghost e poi controllo centro
                
                nuovo_stato(riga_centro, colonna_centro) = cell_type::pacman;
                push_back(nuovo_stato);
                return;
              
            }else{ //panic_mode_timer > 0
                int nr_ghost = 0; 
                for(uint32_t i = 0; i < nuovo_stato.get_size(); i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size(); j++){
                        if(nuovo_stato(i, j) == cell_type::ghost1){
                           nr_ghost++;
                        }else if(nuovo_stato(i, j) == cell_type::ghost2){
                           nr_ghost++;
                        }else if(nuovo_stato(i, j) == cell_type::ghost3){
                           nr_ghost++;
                        }else if(nuovo_stato(i, j) == cell_type::ghost4){
                           nr_ghost++;
                        }
                    }
                }
                int ghost_presenti = 5 - nr_ghost;
                if(ghost_presenti  == 1){
                    score += 200;
                    nuovo_stato.set_score(score);
                }else if(ghost_presenti == 2){
                    score += 400;
                    nuovo_stato.set_score(score);
                }else if(ghost_presenti == 3){
                    score += 800;
                    nuovo_stato.set_score(score);
                }else if(ghost_presenti == 4){
                    score += 1600;
                    nuovo_stato.set_score(score);
                }
                nuovo_stato(riga, colonna) = cell_type::empty;//elimino pacman dalla vecchia posizione -> la cella diventa vuota
                nuovo_stato(nuova_riga, nuova_colonna) = cell_type::pacman;//pacman entra nella poszione del fantasma, cosi il fantasma sprisce automaticamente
                nuovo_stato.set_panic_countdown(panic_mode_timer);
                push_back(nuovo_stato);
                return;
            }

    } else {
            //Moviemento  normale di pac-man
             /*Gestione personaggio pacman: per prima verifico il contenuto della cella di destinazione.
                Se la cella di destinazione è: vuota -> non succede nulla, semplicemente mi sposto; 
                è pellet: mangio il pellet, aggiorno il numero di pellet rimasti e aggiorno lo score; ecc.
                Devo prima liberare la cella attuale di pacman e poi assegnare alla cella di destinazione 
                pacman
                */
            if(contenuto_cella == cell_type::empty){
                nuovo_stato(riga, colonna) = cell_type::empty;
                nuovo_stato(nuova_riga, nuova_colonna) = cell_type::pacman;
                nuovo_stato.set_panic_countdown(panic_mode_timer);
                push_back(nuovo_stato);
                return;
            }else if(contenuto_cella == cell_type::pellet){
                nuovo_stato(riga, colonna) = cell_type::empty;
                nuovo_stato(nuova_riga, nuova_colonna) = cell_type::pacman;
                pallets_left--;
                nuovo_stato.set_pellets_left(pallets_left);
                score += 10;
                nuovo_stato.set_score(score);
                nuovo_stato.set_panic_countdown(panic_mode_timer);
                push_back(nuovo_stato);
                return;
            }else if(contenuto_cella == cell_type::power_pellet){
                nuovo_stato(riga, colonna) = cell_type::empty;
                nuovo_stato(nuova_riga, nuova_colonna) = cell_type::pacman;
                pallets_left--;
                nuovo_stato.set_pellets_left(pallets_left);
                score += 50;
                nuovo_stato.set_score(score);
                panic_mode_timer = PANIC_RESET;
                nuovo_stato.set_panic_countdown(panic_mode_timer);
                push_back(nuovo_stato);
                return;
            }
        }

    }else if(who == cell_type::ghost1 || who == cell_type::ghost2 || who == cell_type::ghost3 || who == cell_type::ghost4){
        //Se il ghost si sposta in una cella vuota, rimane in quella cella
        if(contenuto_cella == cell_type::empty){
            nuovo_stato(riga, colonna) = cell_type::empty;
            nuovo_stato(nuova_riga, nuova_colonna) = who;
            nuovo_stato.set_panic_countdown(panic_mode_timer);
            push_back(nuovo_stato);
            return;
        }

        if(contenuto_cella ==  cell_type::pacman){
            if(panic_mode_timer == 0){
                if(lives_pacman > 0){
                    lives_pacman--;
                }
                nuovo_stato.set_lives(lives_pacman);
                nuovo_stato(riga, colonna) = cell_type::empty; //Tolgo il ghost dalla vecchia posizione
                nuovo_stato(nuova_riga, nuova_colonna) = cell_type::empty; //Elimino Pac-Man dalla cella dove ghost e pacman si sono incontrati, ovvero cella di destinazione
                int riga_centro = size_griglia/2; 
                int colonna_centro = size_griglia/2;
                cell_type  centro = nuovo_stato(riga_centro, colonna_centro); //Rimetto pacman in centro

                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost1){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(0,0) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(0,0) = cell_type::ghost1;
                    } else if(nuovo_stato(0,0) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(0, 0) = cell_type::ghost1;
                    } else if(nuovo_stato(0,0) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(0, 0) = cell_type::ghost1;
                    } else{//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(0,0);
                        nuovo_stato(0,0) = cell_type::ghost1;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;

                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost2){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(0,size_griglia-1) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(0,size_griglia-1) = cell_type::ghost2;
                    } else if(nuovo_stato(0,size_griglia-1) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(0, size_griglia-1) = cell_type::ghost2;
                    } else if(nuovo_stato(0,size_griglia-1) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(0, size_griglia-1) = cell_type::ghost2;
                    } else{//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(0,size_griglia-1);
                        nuovo_stato(0,size_griglia-1) = cell_type::ghost2;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;

                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost3){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(size_griglia-1,0) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(size_griglia-1,0) = cell_type::ghost3;
                    } else if(nuovo_stato(size_griglia-1,0) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(size_griglia-1, 0) = cell_type::ghost3;
                    } else if(nuovo_stato(size_griglia-1,0) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(size_griglia-1, 0) = cell_type::ghost3;
                    } else{//else: se trovo un ghost switch posizione
                        cell_type tmp = nuovo_stato(size_griglia-1, 0);
                        nuovo_stato(size_griglia-1, 0) = cell_type::ghost3;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }
                ghost_trovato = false;
               
                for(uint32_t i = 0; i < nuovo_stato.get_size() && !ghost_trovato; i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size() && !ghost_trovato; j++){
                        if(nuovo_stato(i, j) == cell_type::ghost4){
                            riga_g = i;
                            colonna_g = j;
                            ghost_trovato = true;
                        }
                    }
                }

                if(ghost_trovato){
                    if(nuovo_stato(size_griglia-1,size_griglia-1) == cell_type::empty){
                        nuovo_stato(riga_g, colonna_g) = cell_type::empty;
                        nuovo_stato(size_griglia-1,size_griglia-1) = cell_type::ghost4;
                    } else if(nuovo_stato(size_griglia-1,size_griglia-1) == cell_type::pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::pellet;
                        nuovo_stato(size_griglia-1,size_griglia-1) = cell_type::ghost4;
                    } else if(nuovo_stato(size_griglia-1,size_griglia-1) == cell_type::power_pellet){
                        nuovo_stato(riga_g, colonna_g) = cell_type::power_pellet;
                        nuovo_stato(size_griglia-1,size_griglia-1) = cell_type::ghost4;
                    } else {
                        cell_type tmp = nuovo_stato(size_griglia-1, size_griglia-1);
                        nuovo_stato(size_griglia-1, size_griglia-1) = cell_type::ghost4;
                        nuovo_stato(riga_g, colonna_g) = tmp;
                    }
                }

                ghost_trovato = false;

                //Controllo contenuto della cella in centro
                if(centro == cell_type::empty){
                }else if(centro == cell_type::pellet){
                    pallets_left--;
                    nuovo_stato.set_pellets_left(pallets_left);
                    score += 10;
                    nuovo_stato.set_score(score);
                }else if(centro == cell_type::power_pellet){
                    pallets_left--;
                    nuovo_stato.set_pellets_left(pallets_left);
                    score += 50;
                    nuovo_stato.set_score(score);
                    panic_mode_timer = PANIC_RESET;//panic reset + 1 idea?-> sbagliato. Prima ricavo il panic_contdown dello stato precedente:
                    nuovo_stato.set_panic_countdown(panic_mode_timer);
                    //una volta che pacman ha mangiato un power pellet -> sovrascrivo il panic-contdowm con panic-reset
                }
                nuovo_stato(riga_centro, colonna_centro) = cell_type::pacman;
                nuovo_stato.set_panic_countdown(panic_mode_timer);
                push_back(nuovo_stato);
                return;

            }else {//panic_mode_timer > 0
                int nr_ghost = 0; 
                for(uint32_t i = 0; i < nuovo_stato.get_size(); i++){
                    for(uint32_t j = 0; j < nuovo_stato.get_size(); j++){
                        if(nuovo_stato(i, j) == cell_type::ghost1){
                           nr_ghost++;
                        }else if(nuovo_stato(i, j) == cell_type::ghost2){
                           nr_ghost++;
                        }else if(nuovo_stato(i, j) == cell_type::ghost3){
                           nr_ghost++;
                        }else if(nuovo_stato(i, j) == cell_type::ghost4){
                           nr_ghost++;
                        }
                    }
                }

                int ghost_presenti = 5 - nr_ghost;//4 - (number of ghosts on the grid before the eat) + 1.
                if(ghost_presenti  == 1){
                    score += 200;
                    nuovo_stato.set_score(score);
                }else if(ghost_presenti == 2){
                    score += 400;
                    nuovo_stato.set_score(score);
                }else if(ghost_presenti == 3){
                    score += 800;
                    nuovo_stato.set_score(score);
                }else if(ghost_presenti == 4){
                    score += 1600;
                    nuovo_stato.set_score(score);
                }
                //Nel ramo dei ghost, riga e colonna sono la posizione del ghost 
                nuovo_stato(riga, colonna) = cell_type::empty;//eliminio il ghost
                nuovo_stato.set_panic_countdown(panic_mode_timer);
                push_back(nuovo_stato);
                return;
            }
        }
    }
}

    /*
      Winning / losing. The game is won when pellets_left reaches 0; lost when lives reaches 0. It is up to the caller to stop the game by checking the 
      return value of the win method. (The caller might do further moves even if there are no pellets left.)
      Win verrà controllato dal chiamante
    */
