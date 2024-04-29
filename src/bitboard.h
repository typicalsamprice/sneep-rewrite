#pragma once
#include "types.h"
#include <string>

extern Bitboard BETWEEN_BB[64][64];
extern Bitboard LINE_BB[64][64];
extern int DIST[64][64];

extern Bitboard RAYS[64][8];

void initialize();
std::string pretty(Bitboard bb);

Bitboard attacks(Square s, Piece p, Bitboard occ = 0);

Bitboard line(Square a, Square b);
Bitboard between(Square a, Square b);

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
