#include <iostream>

#include "types.h"
#include "bitboard.h"

int main(int argc, char *argv[]) {
    (void) argc, (void) argv;

    initialize();

    Bitboard occ = bb_from(A2) | bb_from(E3) | bb_from(E5);

    std::cout << pretty(bb_from(E4)) << std::endl;
    std::cout << pretty(shift<North>(E4)) << std::endl;

    std::cout << pretty(RAYS[E4][South] & bb_from(E3)) << std::endl;
    std::cout << "Whole thing: (Rook E4, no occ)\n" << pretty(attacks(E4, Rook + White)) << std::endl;
    std::cout << "Whole thing: (Rook E4, occ A2 E3 E5)\n" << pretty(attacks(E4, Rook + White, occ)) << std::endl;

    return 0;
}
