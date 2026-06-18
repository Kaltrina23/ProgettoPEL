#include "pacman.hpp"

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
    if(s < 4 || !( (s & (s - 1)) == 0 )) throw pacman_exception("Dimensione non valida");
    
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
    m_grid = rhs.m_grid; //rubo il puntatore, non alloco nuova memori, non creo nuove celle
    
    rhs.m_size = 0;
    rhs.m_score = 0;
    rhs.m_lives = 0;
    rhs.m_pellets_left = 0;
    rhs.m_panic_countdown = 0;
    rhs.m_grid = nullptr;
}

/*game_state::game_state(game_state&& rhs) 
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

    if(this == &rhs) return *this; //*this esisite già e dobbiamo liberarla perchè potrebbe avere una graigli allocata. sef-assigemnet(gs = gs), non succede nulla
    
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
    
    if(m_grid != nullptr) throw pacman_exception("Dimensione non valida");

    if(s == 0){
        m_size = 0;
        return;
    }
    
    if(s < 4 || !( (s & (s - 1)) == 0 )) {
        m_size = s;
        m_grid = new cell_type*[s];
        for(unsigned i = 0; i < s; i++){
            m_grid[i] = new cell_type[s];
            for(unsigned j = 0; j < s; ++j){
                m_grid[i][j] = cell_type::empty; 
            }
        }
    }else{
        throw pacman_exception("Dimensione non valida");
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
        throw pacman_exception("Dimensione non valida");
    } else {
        return m_grid[i][j];
    }
}

//Restituisco un valore perchè l'oggeto è const
cell_type game_state::operator()(uint32_t i, uint32_t j) const{
    if(i >= m_size || j >= m_size){
        throw pacman_exception("Dimensione non valida");
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
Questa espressione chiama il tuo operator==:
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

            switch(m_grid[i][j]){

                case cell_type::empty:
                    os << ' ';
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