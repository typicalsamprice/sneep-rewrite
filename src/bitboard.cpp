#include "types.h"
#include "bitboard.h"


Bitboard BETWEEN_BB[64][64] = {};
Bitboard LINE_BB[64][64] = {};
Bitboard SLIDERS_BB[64][8] = {}; // One in each direction
int DIST[64][64] = {};

void initialize() {
    for (Square s1 = A1; s1 <= H8; ++s1) {
        for (Square s2 = A1; s2 <= H8; ++s2) {

            DIST[s1][s2] = DIST[s2][s1] = std::max(distance<File>(s1, s2), distance<Rank>(s1, s2));

        }
    }
}
