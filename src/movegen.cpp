#include "movegen.h"
#include "bitboard.h"
#include "types.h"

template <Color Us> vector<Move> generate(const Position &pos) {
  vector<Move> moves;

  Square king = lsb(pos.pieces(Us, King));

  bool IC = pos.inCheck();

  // Capture targets
  Bitboard targets = pos.pieces(~Us);
  if (IC) {
    int c = popcount(pos.checkers());
    assert(c > 0);
    if (c == 1) {
      targets &= between(king, lsb(pos.checkers())) | lsb(pos.checkers());
    } else {
      assert(c == 2);
      // Cannot capture now, have to evade.
      targets = 0;
    }
  }

  Bitboard pawns = pos.pieces(Us, Pawn);
  Bitboard seventh = pawns & bb_from(relative_to(Rank_7, Us));
  Bitboard rest = pawns ^ seventh;
  assert((pawns & bb_from(relative_to(Rank_8, Us))) == 0);
  assert((pawns & bb_from(relative_to(Rank_1, Us))) == 0);

  constexpr Direction Up = Us == White ? North : South;
  constexpr Direction Down = Us == White ? South : North;

  return moves;
}

void add_promotions(vector<Move> &moves, Square from, Square to) {
  moves.push_back(Move(from, to, Knight));
  moves.push_back(Move(from, to, Bishop));
  moves.push_back(Move(from, to, Rook));
  moves.push_back(Move(from, to, Queen));
}
