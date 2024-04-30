#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "position.h"

int main(int argc, char *argv[]) {
    (void) argc, (void) argv;

    initialize();

    Position p;
    std::cout << p << std::endl;

    p.do_move(Move(E2, E4));
    std::cout << p << std::endl;
    p.do_move(Move(E7, E5));
    std::cout << p << std::endl;

    return 0;
}
