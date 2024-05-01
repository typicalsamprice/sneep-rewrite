#include "uci.h"
#include "bitboard.h"
#include "types.h"
#include <cstring>

Square parse_square(const char *);

Move parse_move(std::string move, const Position &pos) {
  const char *cs = move.c_str();
  ssize_t l = strlen(cs);
  assert(l == 4 || l == 5);

  Square a = parse_square(cs);
  Square b = parse_square(cs + 2);

  Color us = pos.to_move();

  if (cs[4] != 0) {
    // It is a promotion
    PieceT pt = piecet_map_get(cs[4]);
    // Not foolproof
    assert(pos.piece_on(a) == Piece(Pawn, us));
    assert(rank_of(a) == relative_to(Rank_7, us));
    assert(rank_of(b) == relative_to(Rank_8, us));
    assert(distance(a, b) == 1);

    return Move(a, b, pt);
  }

  if (pos.piece_on(a) == Piece(Pawn, us)) {
    if (b == pos.ep())
      return Move(a, b, EnPassant);
  } else if (pos.piece_on(a) == Piece(King, us)) {
    if (distance(a, b) == 2)
      return Move(a, b, Castle);
  }

  return Move(a, b);
}

Square parse_square(const char *str) {
  File f = File(str[0] - 'a');
  Rank r = Rank(str[1] - '1');
  return make_square(f, r);
}
