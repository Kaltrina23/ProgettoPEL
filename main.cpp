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
        game_state gs(8);

        assert(gs.get_size() == 8);
    }

    std::cout << "OK\n";


    std::cout << "TEST 3 - grid initialized to empty\n";
    {
        game_state gs(8);

        for(unsigned i = 0; i < 8; i++){
            for(unsigned j = 0; j < 8; j++){
                assert(gs(i,j) == cell_type::empty);
            }
        }
    }

    std::cout << "OK\n";


    std::cout << "TEST 4 - operator()\n";
    {
        game_state gs(8);

        gs(3,4) = cell_type::pacman;

        assert(gs(3,4) == cell_type::pacman);
    }

    std::cout << "OK\n";


    std::cout << "TEST 5 - copy constructor\n";
    {
        game_state a(8);

        a(1,1) = cell_type::pacman;

        game_state b(a);

        a(1,1) = cell_type::ghost1;

        assert(b(1,1) == cell_type::pacman);
    }

    std::cout << "OK\n";


    std::cout << "TEST 6 - copy assignment\n";
    {
        game_state a(8);
        a(2,2) = cell_type::ghost1;

        game_state b(8);

        b = a;

        a(2,2) = cell_type::empty;

        assert(b(2,2) == cell_type::ghost1);
    }

    std::cout << "OK\n";


    std::cout << "TEST 7 - move constructor\n";
    {
        game_state a(8);

        a(1,1) = cell_type::pacman;

        game_state b(std::move(a));

        assert(b(1,1) == cell_type::pacman);
    }

    std::cout << "OK\n";


    std::cout << "TEST 8 - move assignment\n";
    {
        game_state a(8);

        a(5,5) = cell_type::ghost4;

        game_state b;

        b = std::move(a);

        assert(b(5,5) == cell_type::ghost4);
    }

    std::cout << "OK\n";


    std::cout << "TEST 9 - operator==\n";
    {
        game_state a(8);
        game_state b(8);

        assert(a == b);

        a(0,0) = cell_type::pacman;

        assert(a != b);
    }

    std::cout << "OK\n";


    std::cout << "TEST 10 - print_ascii_art\n";
    {
        game_state gs(8);

        gs.set_score(0);
        gs.set_lives(3);
        gs.set_pellets_left(20);

        gs(0,0) = cell_type::ghost1;
        gs(0,7) = cell_type::ghost2;

        gs(5,0) = cell_type::ghost3;
        gs(6,7) = cell_type::ghost4;

        gs(2,2) = cell_type::pacman;

        // pellet
        gs(0,2) = cell_type::pellet;
        gs(0,3) = cell_type::pellet;
        gs(0,4) = cell_type::pellet;

        gs(2,3) = cell_type::pellet;
        gs(2,4) = cell_type::pellet;
        gs(2,5) = cell_type::pellet;

        gs(5,1) = cell_type::pellet;
        gs(5,2) = cell_type::pellet;
        gs(5,3) = cell_type::pellet;

        // muri
        gs(1,1) = cell_type::wall;
        gs(1,2) = cell_type::wall;
        gs(1,3) = cell_type::wall;
        gs(1,4) = cell_type::wall;

        gs(1,6) = cell_type::wall;
        gs(1,7) = cell_type::wall;

        gs(2,1) = cell_type::wall;
        gs(3,1) = cell_type::wall;

        gs(4,6) = cell_type::wall;
        gs(5,6) = cell_type::wall;

        // power pellet
        gs(6,3) = cell_type::power_pellet;

        gs.print_ascii_art(std::cout);
    }

    std::cout << "\nALL TESTS PASSED\n";

    return 0;
}

