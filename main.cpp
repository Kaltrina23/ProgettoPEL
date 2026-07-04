#include <iostream>
#include <cassert>
#include "pacman.hpp"
//#include "provaPacman.cpp"

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

    std::cout << "OK\n";
    
    int* p = new int(5);
    delete p;

    std::cout << *p << std::endl;

    return 0;
}

