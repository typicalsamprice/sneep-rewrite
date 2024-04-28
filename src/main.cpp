#include <iostream>

#include "types.h"
#include "bitboard.h"

int main(int argc, char *argv[]) {
    (void) argc, (void) argv;

    initialize();

    std::cout << pretty(bb_from(E4)) << std::endl;
    std::cout << pretty(shift<North>(E4)) << std::endl;

    std::cout << "Whole thing: (Rook E4, no occ)\n" << pretty(attacks(E4, Rook + White)) << std::endl;

    return 0;
}
