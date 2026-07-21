#include <iostream>
#include <cassert>
#include "pacman.hpp"


int main() {
    

    std::cout << "TEST 1 - default constructor\n";
    {
        game_state gs;

        assert(gs.get_size() == 0);
        assert(gs.get_score() == 0);
        assert(gs.get_lives() == 0);
    }

    std::cout << "OK\n";


    std::cout << "TEST 2 - size constructor\n";
    {
        game_state gs(4);

        assert(gs.get_size() == 4);
    }

    std::cout << "OK\n";


    std::cout << "TEST 3 - grid initialized to empty\n";
    {
        game_state gs(16);

        for(unsigned i = 0; i < 16; i++){
            for(unsigned j = 0; j < 16; j++){
                assert(gs(i,j) == cell_type::empty);
            }
        }
    }

    std::cout << "OK\n";


    std::cout << "TEST 4 - operator()\n";
    {
        game_state gs(16);

        gs(3,4) = cell_type::pacman;

        assert(gs(3,4) == cell_type::pacman);
    }

    std::cout << "OK\n";


    std::cout << "TEST 5 - copy constructor\n";
    {
        game_state a(16);

        a(1,1) = cell_type::pacman;

        game_state b(a);

        a(1,1) = cell_type::ghost1;

        assert(b(1,1) == cell_type::pacman);
    }

    std::cout << "OK\n";


    std::cout << "TEST 6 - copy assignment\n";
    {
        game_state a(16);
        a(2,2) = cell_type::ghost1;

        game_state b(16);

        b = a;

        a(2,2) = cell_type::empty;

        assert(b(2,2) == cell_type::ghost1);
    }

    std::cout << "OK\n";


    std::cout << "TEST 7 - move constructor\n";
    {
        game_state a(16);

        a(1,1) = cell_type::pacman;

        game_state b(std::move(a));

        assert(b(1,1) == cell_type::pacman);
    }

    std::cout << "OK\n";


    std::cout << "TEST 8 - move assignment\n";
    {
        game_state a(16);

        a(5,5) = cell_type::ghost4;

        game_state b;

        b = std::move(a);

        assert(b(5,5) == cell_type::ghost4);
    }

    std::cout << "OK\n";


    std::cout << "TEST 9 - operator==\n";
    {
        game_state a(16);
        game_state b(16);

        assert(a == b);

        a(0,0) = cell_type::pacman;

        assert(a != b);
    }

    std::cout << "OK\n";


    std::cout << "TEST 10 - print_ascii_art\n";
        {
        game_state gs(16);

        gs.set_score(150);
        gs.set_lives(3);
        gs.set_pellets_left(30);
        gs.set_panic_countdown(12);

        // Ghosts
        gs(0,0)   = cell_type::ghost1;
        gs(0,15)  = cell_type::ghost2;
        gs(15,0)  = cell_type::ghost3;
        gs(15,15) = cell_type::ghost4;

        // Pacman
        gs(8,8) = cell_type::pacman;

        // Pellet sulla diagonale
        for(unsigned i=1;i<15;i++)
        gs(i,i)=cell_type::pellet;

        // Power pellet
        gs(3,12)=cell_type::power_pellet;
        gs(12,3)=cell_type::power_pellet;

        // Muro orizzontale
        for(unsigned j=2;j<14;j++)
        gs(5,j)=cell_type::wall;

        // Muro verticale
        for(unsigned i=6;i<15;i++)
        gs(i,10)=cell_type::wall;

        // Controllo che i valori siano realmente scritti
        std::cout << "Prima della stampa\n";

        gs.print_ascii_art(std::cout);

        std::cout << "Dopo la stampa\n";

        gs.print_ascii_art(std::cout);
    }

    //Test matrice completamente vuota
    std::cout << "\nTEST 11 - Empty grid\n";
    {
        game_state gs(16);
        gs.print_ascii_art(std::cout);
    }

    //Tutti i pellet
    std::cout << "\nTEST 12 - Full pellets\n";
    {
        game_state gs(16);

        for(unsigned i=0;i<16;i++)
            for(unsigned j=0;j<16;j++)
                gs(i,j)=cell_type::pellet;

        gs.print_ascii_art(std::cout);
    }

    //Tutti i muri
    std::cout << "\nTEST 13 - Full walls\n";
    {
        game_state gs(16);

        for(unsigned i=0;i<16;i++)
            for(unsigned j=0;j<16;j++)
                gs(i,j)=cell_type::wall;

        gs.print_ascii_art(std::cout);
    }

    //Tutti i bordi
    std::cout << "\nTEST 14 - Borders\n";
    {
        game_state gs(16);

        for(unsigned i=0;i<16;i++)
        {
            gs(0,i)=cell_type::wall;
            gs(15,i)=cell_type::wall;
            gs(i,0)=cell_type::wall;
            gs(i,15)=cell_type::wall;
        }

        gs.print_ascii_art(std::cout);
    }

    std::cout << "\nALL TESTS PASSED\n";
    
    //TEST PER PACMAN

    //Test 1 - Default constructor
    std::cout << "\nPACMAN TEST 1 - default constructor\n";
    {
        pacman p;
        assert(p.size() == 0);
        assert(p.length() == 0);
        assert(p.empty() == true);
    }

    std::cout << "OK\n";

    //Test 2 - Costruttore con size
    std::cout << "\nPACMAN TEST 2 - size constructor\n";
    {
        pacman p(16);

        assert(p.size() == 16);
        assert(p.length() == 0);
        assert(p.empty() == true);
    }

    std::cout << "OK\n";

    //Test 3 - Size non valida
    std::cout << "\nPACMAN TEST 3 - invalid size\n";
    {
        try{
            pacman p(5);
            assert(false); // non deve arrivare qui
        }
        catch(const pacman_exception&){
            std::cout << "OK\n";
        }
    }

    //Test 4 - front() su lista vuota
    std::cout << "\nPACMAN TEST 4 - front empty\n";
    {
        pacman p(16);

        try{
            p.front();
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout << "OK\n";
        }
    }

    //Test 5 - back() su lista vuota
    std::cout << "\nPACMAN TEST 5 - back empty\n";
    {
        pacman p(16);

        try{
            p.back();
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout << "OK\n";
        }
    }

    //Copy constructor su lista vuota
    std::cout << "\nPACMAN TEST 6 - copy constructor empty\n";
    {
        pacman a(16);

        pacman b(a);

        assert(b.size() == 16);
        assert(b.length() == 0);
        assert(b.empty());
    }

    std::cout << "OK\n";

    //Move constructor su lista vuota
    std::cout << "\nPACMAN TEST 7 - move constructor empty\n";
    {
        pacman a(16);

        pacman b(std::move(a));

        assert(b.size() == 16);
        assert(b.length() == 0);
        assert(b.empty());

        assert(a.size() == 0);
        assert(a.length() == 0);
        assert(a.empty());
    }

    std::cout << "OK\n";

    //Copy assignment
    std::cout << "\nPACMAN TEST 8 - copy assignment empty\n";
    {
        pacman a(16);
        pacman b;

        b = a;

        assert(b.size() == 16);
        assert(b.length() == 0);
        assert(b.empty());
    }

    std::cout << "OK\n";

    //Move assignment
    std::cout << "\nPACMAN TEST 9 - move assignment empty\n";
    {
        pacman a(16);
        pacman b;

        b = std::move(a);

        assert(b.size() == 16);
        assert(b.length() == 0);

        assert(a.size() == 0);
        assert(a.length() == 0);
    }

    std::cout << "\nPACMAN TEST 10 - push_back\n";
    {
        pacman p(16);

        game_state gs(16);
        gs(8,8) = cell_type::pacman;

        p.push_back(gs);

        assert(p.length() == 1);
        assert(!p.empty());

        assert(p.front()(8,8) == cell_type::pacman);
        assert(p.back()(8,8) == cell_type::pacman);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 11 - push_back multiple\n";
    {
        pacman p(16);

        game_state gs1(16);
        gs1(1,1) = cell_type::pacman;

        game_state gs2(16);
        gs2(2,2) = cell_type::ghost1;

        p.push_back(gs1);
        p.push_back(gs2);

        assert(p.length() == 2);

        assert(p.front()(1,1) == cell_type::pacman);
        assert(p.back()(2,2) == cell_type::ghost1);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 12 - iterator\n";
    {
        pacman p(16);

        game_state gs1(16);
        gs1(1,1) = cell_type::pacman;

        game_state gs2(16);
        gs2(2,2) = cell_type::ghost1;

        p.push_back(gs1);
        p.push_back(gs2);

        pacman::iterator it = p.begin();

        assert((*it)(1,1) == cell_type::pacman);

        ++it;

        assert((*it)(2,2) == cell_type::ghost1);
    }

    std::cout << "OK\n";


    std::cout << "\nPACMAN TEST 13 - const_iterator\n";
    {
        pacman p(16);

        game_state gs1(16);
        gs1(3,3) = cell_type::pacman;

        game_state gs2(16);
        gs2(4,4) = cell_type::ghost2;

        p.push_back(gs1);
        p.push_back(gs2);

        const pacman& cp = p;

        pacman::const_iterator it = cp.begin();

        assert((*it)(3,3) == cell_type::pacman);

        ++it;

        assert((*it)(4,4) == cell_type::ghost2);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 14 - move pacman\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8) = cell_type::pacman;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        assert(p.length() == 2);

        game_state const& last = p.back();

        assert(last(8,8) == cell_type::empty);
        assert(last(8,9) == cell_type::pacman);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 15 - move ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0) = cell_type::ghost1;

        p.push_back(gs);

        p.move(cell_type::ghost1,1,0);

        game_state const& last = p.back();

        assert(last(0,0) == cell_type::empty);
        assert(last(1,0) == cell_type::ghost1);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 16 - move against wall\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8) = cell_type::pacman;
        gs(8,9) = cell_type::wall;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last = p.back();

        assert(last(8,8) == cell_type::pacman);
        assert(last(8,9) == cell_type::wall);

        assert(p.length() == 2);
    }

    std::cout << "OK\n";
    std::cout << "\nPACMAN TEST 17 - eat pellet\n";
    {
        pacman p(16);

        game_state gs(16);
        gs.set_pellets_left(1);

        gs(8,8) = cell_type::pacman;
        gs(8,9) = cell_type::pellet;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last = p.back();

        assert(last(8,9) == cell_type::pacman);
        assert(last.get_score() == 10);
        assert(last.get_pellets_left() == 0);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 18 - eat power pellet\n";
    {
        pacman p(16);

        game_state gs(16);
        gs.set_pellets_left(1);

        gs(8,8) = cell_type::pacman;
        gs(8,9) = cell_type::power_pellet;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last = p.back();

        assert(last(8,9) == cell_type::pacman);
        assert(last.get_score() == 50);
        assert(last.get_pellets_left() == 0);
        assert(last.get_panic_countdown() == PANIC_RESET);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 19 - ghost into pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0) = cell_type::ghost1;
        gs(1,0) = cell_type::pellet;

        p.push_back(gs);

        p.move(cell_type::ghost1,1,0);

        game_state const& last = p.back();

        assert(last(1,0) == cell_type::ghost1);
        assert(last(0,0) == cell_type::pellet);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 20 - ghost into power pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0) = cell_type::ghost1;
        gs(1,0) = cell_type::power_pellet;

        p.push_back(gs);

        p.move(cell_type::ghost1,1,0);

        game_state const& last = p.back();

        assert(last(1,0) == cell_type::ghost1);
        assert(last(0,0) == cell_type::power_pellet);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 21 - pacman meets ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs.set_lives(3);

        gs(8,8) = cell_type::pacman;
        gs(8,9) = cell_type::ghost1;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last = p.back();

        assert(last.get_lives() == 2);
    }
    
    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 22 - pacman eats ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs.set_panic_countdown(5);

        gs(8,8)=cell_type::pacman;

        gs(8,9)=cell_type::ghost1;
        gs(0,15)=cell_type::ghost2;
        gs(15,0)=cell_type::ghost3;
        gs(15,15)=cell_type::ghost4;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last(8,9)==cell_type::pacman);
        assert(last.get_score()==200);
    }
    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 23 - ghost meets pacman\n";
    {
        pacman p(16);

        game_state gs(16);

        gs.set_lives(3);

        gs(0,0) = cell_type::ghost1;
        gs(1,0) = cell_type::pacman;

        p.push_back(gs);

        p.move(cell_type::ghost1,1,0);

        game_state const& last = p.back();

        assert(last.get_lives() == 2);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 24 - ghost meets pacman panic\n";
    {
        pacman p(16);

        game_state gs(16);

        gs.set_panic_countdown(5);

        gs(1,0) = cell_type::pacman;

        gs(0,0)  = cell_type::ghost1;
        gs(0,15) = cell_type::ghost2;
        gs(15,0) = cell_type::ghost3;
        gs(15,15)= cell_type::ghost4;
        p.push_back(gs);

        p.move(cell_type::ghost1,1,0);

        game_state const& last = p.back();

        assert(last.get_score() == 200);
        //std::cout << "Score = " << last.get_score() << '\n';
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 25 - out of bounds\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0) = cell_type::pacman;

        p.push_back(gs);

        p.move(cell_type::pacman,-1,0);

        game_state const& last = p.back();

        assert(last(0,0) == cell_type::pacman);
        assert(p.length() == 2);
    }

    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 26 - empty history\n";
    {
        pacman p(16);

        try{
            p.move(cell_type::pacman,1,0);
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout << "OK\n";
        }
    }

    std::cout << "\nPACMAN TEST 27 - invalid direction\n";
    {
        pacman p(16);

        game_state gs(16);
        gs(8,8) = cell_type::pacman;

        p.push_back(gs);

        try{
            p.move(cell_type::pacman,1,1);
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout << "OK\n";
        }
    }

    std::cout << "\nPACMAN TEST 28 - push_back wrong size\n";
    {
        pacman p(16);

        game_state gs1(16);
        game_state gs2(8);

        p.push_back(gs1);

        try{
            p.push_back(gs2);
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout << "OK\n";
        }
    }

    std::cout << "\nCasi di test limite\n";

    std::cout << "\nPACMAN TEST 29 - self assignment\n";
    {
        pacman p(16);

        game_state gs(16);
        gs(8,8)=cell_type::pacman;

        p.push_back(gs);

        p = p;

        assert(p.length()==1);
        assert(p.front()(8,8)==cell_type::pacman);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 30 - self move assignment\n";
    {
        pacman p(16);

        game_state gs(16);
        gs(8,8)=cell_type::pacman;

        p.push_back(gs);

        p = std::move(p);

        assert(p.length()==1);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 31 - begin end empty\n";
    {
        pacman p(16);

        assert(p.begin()==p.end());
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 32 - dereference end\n";
    {
        pacman p(16);

        try{
            *p.end();
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout<<"OK\n";
        }
    }

    std::cout << "\nPACMAN TEST 33 - missing ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;

        p.push_back(gs);

        try{
            p.move(cell_type::ghost1,1,0);
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout<<"OK\n";
        }
    }

    std::cout << "\nPACMAN TEST 34 - missing pacman\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0)=cell_type::ghost1;

        p.push_back(gs);

        try{
            p.move(cell_type::pacman,1,0);
            assert(false);
        }
        catch(const pacman_exception&){
            std::cout<<"OK\n";
        }
    }

    std::cout << "\nPACMAN TEST 35 - last life\n";
    {
        pacman p(16);

        game_state gs(16);

        gs.set_lives(1);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::ghost1;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        auto const& last=p.back();

        assert(last.get_lives()==0);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 36 - stress copy\n";
    {
        pacman p(16);

        for(int i=0;i<100;i++){
            game_state gs(16);
            gs(i%16,i%16)=cell_type::pacman;
            p.push_back(gs);
        }

        pacman q(p);

        assert(q.length()==100);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 37 - stress move\n";
    {
        pacman p(16);

        for(int i=0;i<100;i++){
            game_state gs(16);
            gs(8,8)=cell_type::pacman;
            p.push_back(gs);
        }

        pacman q(std::move(p));

        assert(q.length()==100);
        assert(p.length()==0);
    }
    std::cout<<"OK\n";


    std::cout << "\n-----------------------------------Test per la funzione move-------------------------------------------\n";

    std::cout << "\nPACMAN TEST 38 - move ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0) = cell_type::ghost1;

        p.push_back(gs);

        p.move(cell_type::ghost1,0,1);

        assert(p.length() == 2);

        game_state const& last = p.back();

        assert(last(0,0) == cell_type::empty);
        assert(last(0,1) == cell_type::ghost1);
    }
    std::cout << "OK\n";

    std::cout << "\nPACMAN TEST 39 - wall\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::wall;

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last(8,8)==cell_type::pacman);
        assert(last(8,9)==cell_type::wall);

        assert(p.length()==2);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 40 - out of bounds\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0)=cell_type::pacman;

        p.push_back(gs);

        p.move(cell_type::pacman,-1,0);

        game_state const& last=p.back();

        assert(last(0,0)==cell_type::pacman);

        assert(p.length()==2);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 41 - eat pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::pellet;

        gs.set_pellets_left(1);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last(8,8)==cell_type::empty);
        assert(last(8,9)==cell_type::pacman);

        assert(last.get_score()==10);
        assert(last.get_pellets_left()==0);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 42 - ghost swap pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0)=cell_type::ghost1;
        gs(0,1)=cell_type::pellet;

        gs.set_pellets_left(10);

        p.push_back(gs);

        p.move(cell_type::ghost1,0,1);

        game_state const& last=p.back();

        assert(last(0,1)==cell_type::ghost1);
        assert(last(0,0)==cell_type::pellet);

        assert(last.get_pellets_left()==10);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 43 - ghost swap power pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0)=cell_type::ghost1;
        gs(0,1)=cell_type::power_pellet;

        gs.set_pellets_left(5);

        p.push_back(gs);

        p.move(cell_type::ghost1,0,1);

        game_state const& last=p.back();

        assert(last(0,1)==cell_type::ghost1);
        assert(last(0,0)==cell_type::power_pellet);

        assert(last.get_pellets_left()==5);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 44 - ghost into ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(0,0)=cell_type::ghost1;
        gs(0,1)=cell_type::ghost2;

        p.push_back(gs);

        p.move(cell_type::ghost1,0,1);

        game_state const& last=p.back();

        assert(last(0,0)==cell_type::ghost1);
        assert(last(0,1)==cell_type::ghost2);

        assert(p.length()==2);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 45 - panic countdown\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs.set_panic_countdown(5);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last.get_panic_countdown()==4);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 46 - missing ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;

        p.push_back(gs);

        try{
            p.move(cell_type::ghost1,0,1);
            assert(false);
        }
        catch(pacman_exception const&){
        }
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 47 - eat first ghost\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::ghost1;

        gs(0,0)=cell_type::ghost2;
        gs(0,15)=cell_type::ghost3;
        gs(15,0)=cell_type::ghost4;

        gs.set_panic_countdown(PANIC_RESET+1);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last.get_score()==200);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 48 - ghost into pacman panic\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::ghost1;

        gs(0,0)=cell_type::ghost2;
        gs(0,15)=cell_type::ghost3;
        gs(15,0)=cell_type::ghost4;

        gs.set_panic_countdown(PANIC_RESET+1);

        p.push_back(gs);

        p.move(cell_type::ghost1,0,-1);

        game_state const& last=p.back();

        assert(last(8,8)==cell_type::pacman);
        assert(last(8,9)==cell_type::empty);
        std::cout << "Score = " << last.get_score() << std::endl;
        assert(last.get_score()==200);

    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 49 - lose one life\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::ghost1;

        gs.set_lives(3);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last.get_lives()==2);


        assert(last(8,9)==cell_type::empty);

        assert(last(0,0)==cell_type::ghost1);

        assert(last(8,8)==cell_type::pacman);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 50 - ghost kills pacman\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::ghost1;

        gs.set_lives(3);

        p.push_back(gs);

        p.move(cell_type::ghost1,0,-1);

        game_state const& last=p.back();

        assert(last.get_lives()==2);

        assert(last(0,0)==cell_type::ghost1);

        assert(last(8,8)==cell_type::pacman);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 51 - last life\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::ghost1;

        gs.set_lives(1);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        game_state const& last=p.back();

        assert(last.get_lives()==0);
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 52 - last pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::pellet;

        gs.set_pellets_left(1);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        assert(p.back().get_pellets_left()==0);

        assert(p.back().win());
    }
    std::cout<<"OK\n";

    std::cout << "\nPACMAN TEST 53 - last pellet\n";
    {
        pacman p(16);

        game_state gs(16);

        gs(8,8)=cell_type::pacman;
        gs(8,9)=cell_type::pellet;

        gs.set_pellets_left(1);

        p.push_back(gs);

        p.move(cell_type::pacman,0,1);

        assert(p.back().get_pellets_left()==0);

        assert(p.back().win());
    }
    std::cout<<"OK\n";
    return 0;
}

