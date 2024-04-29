#include "bitboard.h"
#include "types.h"

Bitboard BETWEEN_BB[64][64] = {};
Bitboard LINE_BB[64][64] = {};
int DIST[64][64] = {};

Bitboard PAWN_ATTS[64][2] = {};
Bitboard KNIGHT_ATTS[64] = {};
Bitboard KING_ATTS[64] = {};
Bitboard RAYS[64][8] = {};

template <Color C> constexpr Bitboard pawn_attacks(Square s) {
  return C == White ? shift<NorthEast>(s) | shift<NorthWest>(s)
                    : shift<SouthEast>(s) | shift<SouthWest>(s);
}

constexpr Bitboard knight_attacks(Square s) {
  return shift<East>(shift_n<North>(s, 2) | shift_n<South>(s, 2) |
                     shift<NorthEast>(s) | shift<SouthEast>(s)) |
         shift<West>(shift_n<North>(s, 2) | shift_n<South>(s, 2) |
                     shift<NorthWest>(s) | shift<SouthWest>(s));
}
constexpr Bitboard king_attacks(Square s) {
  return shift<North>(s) | shift<NorthWest>(s) | shift<NorthEast>(s) |
         shift<SouthWest>(s) | shift<SouthEast>(s) | shift<South>(s) |
         shift<East>(s) | shift<West>(s);
}

Bitboard get_slider_atts(Square s, Bitboard occ, bool diag) {
  Bitboard rv = 0;
  Direction dirs[4]{NorthWest, NorthEast, SouthWest, SouthEast};
  // Don't care, this game sucks anyways
  if (!diag) {
    dirs[0] = North;
    dirs[1] = South;
    dirs[2] = East;
    dirs[3] = West;
  }

  for (Direction d : dirs) {
    Bitboard atts = RAYS[s][d];
    Bitboard blockers = occ & atts;
    if (blockers) {
      Square blocking;
      if (bb_from(s) < blockers) {
        blocking = lsb(blockers);
      } else {
        blocking = msb(blockers);
      }
      atts &= ~RAYS[blocking][d];
    }
    rv |= atts;
  }

  return rv;
}

Bitboard attacks(Square s, Piece p, Bitboard occ) {
  switch (p.type) {
  case ALL_TYPES:
    if (ThrowErrs)
      assert(0 && "ALL_TYPES in attacks");
  case NO_TYPE:
    if (ThrowErrs)
      assert(0 && "NO_TYPE in attacks");
  case Pawn:
    return PAWN_ATTS[s][p.color];
  case Knight:
    return KNIGHT_ATTS[s];
  case King:
    return KING_ATTS[s];
  case Bishop:
    return get_slider_atts(s, occ, true);
  case Rook:
    return get_slider_atts(s, occ, false);
  case Queen:
    return get_slider_atts(s, occ, true) | get_slider_atts(s, occ, false);
  }
}

Bitboard line(Square a, Square b) {
  assert(is_ok(a));
  assert(is_ok(b));
  return LINE_BB[a][b];
}
Bitboard between(Square a, Square b) {
  assert(is_ok(a));
  assert(is_ok(b));
  return BETWEEN_BB[a][b];
}

void initialize() {
  for (Square s1 = A1; s1 <= H8; ++s1) {
    for (Square s2 = A1; s2 <= H8; ++s2) {
      DIST[s1][s2] = DIST[s2][s1] =
          std::max(distance<File>(s1, s2), distance<Rank>(s1, s2));
    }

    PAWN_ATTS[s1][White] = pawn_attacks<White>(s1);
    PAWN_ATTS[s1][Black] = pawn_attacks<Black>(s1);

    KNIGHT_ATTS[s1] = knight_attacks(s1);
    KING_ATTS[s1] = king_attacks(s1);

    // Init rays
    for (Direction d : {North, East, South, West, NorthEast, SouthEast,
                        NorthWest, SouthWest}) {
      Bitboard b = 0;
      Bitboard s = shift(s1, d);
      while (s) {
        b |= s;
        s = shift(s, d);
      }

      RAYS[s1][d] = b;
    }

    for (Square s2 = A1; s2 <= H8; ++s2) {
      if (!(file_of(s1) == file_of(s2) || rank_of(s1) == rank_of(s2) ||
            distance<File>(s1, s2) == distance<Rank>(s1, s2)))
        continue;

      if (s1 == s2)
        continue;

      for (Direction d : {North, East, South, West, NorthEast, SouthEast,
                          NorthWest, SouthWest}) {
        if (RAYS[s1][d] & bb_from(s2)) {
          Direction opp = (d % 2 == 0) ? Direction(d + 1) : Direction(d - 1);
          LINE_BB[s1][s2] = LINE_BB[s2][s1] = RAYS[s1][d] | RAYS[s2][opp];
          BETWEEN_BB[s1][s2] = BETWEEN_BB[s2][s1] = RAYS[s1][d] & RAYS[s2][opp];
          break;
        }
      }
    }
  }
}

std::string pretty(Bitboard bb) {
  std::string sep = "+---+---+---+---+---+---+---+---+\n";
  std::string s = sep;
  for (Rank r = Rank_8; r >= Rank_1; --r) {
    for (File f = File_A; f <= File_H; ++f)
      s += bb & bb_from(make_square(f, r)) ? "| X " : "|   ";
    s += "| " + std::to_string(1 + r) + "\n" + sep;
  }
  s += "  a   b   c   d   e   f   g   h\n";
  return s;
}
