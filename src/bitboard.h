#pragma once
#include "types.h"
#include <string>

extern Bitboard BETWEEN_BB[64][64];
extern Bitboard LINE_BB[64][64];
extern Bitboard SLIDERS_BB[64][8]; // One in each direction
extern int DIST[64][64];

extern Bitboard RAYS[64][8];

void initialize();
std::string pretty(Bitboard bb);

Bitboard attacks(Square s, Piece p, Bitboard occ = 0);

template<typename T = Square>
inline int distance(Square s1, Square s2);

template<> inline int distance<File>(Square s1, Square s2) {
  return std::abs(file_of(s1) - file_of(s2));
}
template<> inline int distance<Rank>(Square s1, Square s2) {
  return std::abs(rank_of(s1) - rank_of(s2));
}
template<> inline int distance<Square>(Square s1, Square s2) {
  return DIST[s1][s2];
}