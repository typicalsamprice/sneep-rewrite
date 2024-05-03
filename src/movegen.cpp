#include "movegen.h"
#include "bitboard.h"
#include "types.h"

enum GenType { Evasions, All };

template <Color Us> vector<Move> generate(const Position &pos) {
  vector<Move> moves;

  constexpr Color Them = ~Us;

  GenType gt = pos.inCheck() ? Evasions : All;

  Square king = lsb(pos.pieces(Us, King));
  Bitboard checkers = pos.checkers();

  Bitboard targets =
      gt == Evasions ? (more_than_one(checkers)
                            ? 0
                            : (between(king, lsb(checkers)) | lsb(checkers)))
                     : ~pos.pieces(Us);
  Bitboard empty = ~pos.pieces();

  Bitboard pawns = pos.pieces(Us, Pawn);
  Bitboard seventh = pawns & bb_from(relative_to(Rank_7, Us));
  Bitboard rest = pawns ^ seventh;
  assert((pawns & bb_from(relative_to(Rank_8, Us))) == 0);
  assert((pawns & bb_from(relative_to(Rank_1, Us))) == 0);

  constexpr Direction Up = Us == White ? North : South;
  constexpr Direction Down = Us == White ? South : North;

  if (rest) {
    // Non-promotions
    Bitboard up1 = shift<Up>(rest) & empty;
    Bitboard up2 = shift<Up>(up1 & bb_from(relative_to(Rank_3, Us))) & empty;
    while (up1) {
      Square t = pop_lsb(up1);
      moves.push_back(Move(lsb(shift<Down>(t)), t));
    }
    while (up2) {
      Square t = pop_lsb(up2);
      moves.push_back(Move(lsb(shift_n<Down>(t, 2)), t));
    }
  }

  return moves;
}

void add_promotions(vector<Move> &moves, Square from, Square to) {
  moves.push_back(Move(from, to, Knight));
  moves.push_back(Move(from, to, Bishop));
  moves.push_back(Move(from, to, Rook));
  moves.push_back(Move(from, to, Queen));
}
