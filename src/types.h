/* -*- mode: c++; -*- */
#pragma once
#include <bit>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <map>

#if defined(NDEBUG)
#undef THROW_ERRORS
#undef THROW_MANY_ERRORS
#endif

#if defined(THROW_ERRORS) || defined(THROW_MANY_ERRS)
constexpr bool ThrowErrs = true;
#else
constexpr bool ThrowErrs = false;
#endif

#if defined(THROW_MANY_ERRS)
constexpr bool ExtraErrs = true;
#else
constexpr bool ExtraErrs = false;
#endif

// clang-format off
enum Square {
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8,
  NO_SQUARE
};
// clang-format on

enum Rank { Rank_1, Rank_2, Rank_3, Rank_4, Rank_5, Rank_6, Rank_7, Rank_8 };
enum File { File_A, File_B, File_C, File_D, File_E, File_F, File_G, File_H };

enum Direction {
  North,
  South,
  East,
  West,
  NorthEast,
  SouthWest,
  SouthEast,
  NorthWest
};

enum Color { White, Black };

typedef uint64_t Bitboard;

enum PieceT { NO_TYPE, Pawn, Knight, Bishop, Rook, Queen, King, ALL_TYPES };

static std::map<PieceT, char> PIECET_MAP = {{Pawn, 'p'},   {Knight, 'n'},
                                            {Bishop, 'b'}, {Rook, 'r'},
                                            {Queen, 'q'},  {King, 'k'}};
inline PieceT piecet_map_get(char tok) {
  for (auto e : PIECET_MAP) {
    if (e.second == tok)
      return e.first;
  }
  assert(0 && "no piecet char found");
}

// May be rewritten later
class Piece {
public:
  PieceT type;
  Color color;
  Piece(PieceT t, Color c) : type(t), color(c) {}
  Piece() : type(NO_TYPE), color(White) {}

  bool operator==(const Piece &rhs) const {
    return type == rhs.type && color == rhs.color;
  }

  bool operator!=(const Piece &rhs) const { return !operator==(rhs); }
};

enum CastlePerms {
  WhiteKingside = 0x1,
  WhiteQueenside = 0x2,
  BlackKingside = 0x4,
  BlackQueenside = 0x10,
};

enum MoveT { Normal, EnPassant, Castle, Promotion };

// Can also be condensed, see SF
class Move {
public:
  Square from;
  Square to;
  MoveT flag;
  PieceT prom;
  Move(Square f, Square t, MoveT fl = Normal)
      : from(f), to(t), flag(fl), prom(NO_TYPE) {
    assert(fl != Promotion);
  }
  Move(Square f, Square t, PieceT prom)
      : from(f), to(t), flag(Promotion), prom(prom) {}
};

/* BB funcs */

constexpr Bitboard bb_from(Square s) {
  assert(s != NO_SQUARE);
  return (Bitboard(1) << int(s));
}
constexpr Bitboard bb_from(File f) {
  return Bitboard(0x0101010101010101 << int(f));
}
constexpr Bitboard bb_from(Rank r) { return Bitboard(0xff) << (8 * int(r)); }

template <Direction D> constexpr Bitboard shift(Bitboard bb) {
  return D == North       ? bb << 8
         : D == South     ? bb >> 8
         : D == East      ? ((bb << 1) & ~bb_from(File_A))
         : D == West      ? ((bb >> 1) & ~bb_from(File_H))
         : D == NorthWest ? shift<North>(shift<West>(bb))
         : D == NorthEast ? shift<North>(shift<East>(bb))
         : D == SouthWest ? shift<South>(shift<West>(bb))
                          : shift<South>(shift<East>(bb));
}
template <Direction D> constexpr Bitboard shift_n(Bitboard bb, int iter) {
  return iter <= 0 ? bb : shift_n<D>(shift<D>(bb), iter - 1);
}

constexpr Bitboard shift(Bitboard bb, Direction d) {
  return d == North       ? bb << 8
         : d == South     ? bb >> 8
         : d == East      ? ((bb << 1) & ~bb_from(File_A))
         : d == West      ? ((bb >> 1) & ~bb_from(File_H))
         : d == NorthWest ? shift(shift(bb, West), North)
         : d == NorthEast ? shift(shift(bb, East), North)
         : d == SouthWest ? shift(shift(bb, West), South)
                          : shift(shift(bb, East), South);
}
constexpr Bitboard shift_n(Bitboard bb, int iter, Direction d) {
  return iter <= 0 ? bb : shift_n(shift(bb, d), iter - 1, d);
}

template <Direction D> constexpr Bitboard shift(Square s) {
  return shift<D>(bb_from(s));
}
constexpr Bitboard shift(Square s, Direction d) { return shift(bb_from(s), d); }

template <Direction D> constexpr Bitboard shift_n(Square s, int iter) {
  return shift_n<D>(bb_from(s), iter);
}
constexpr Bitboard shift_n(Square s, int iter, Direction d) {
  return shift_n(bb_from(s), iter, d);
}

constexpr Bitboard shift_no_mask(Bitboard bb, Direction d) {
  return d == North       ? bb << 8
         : d == South     ? bb >> 8
         : d == East      ? bb << 1
         : d == West      ? bb >> 1
         : d == NorthWest ? shift_no_mask(shift_no_mask(bb, West), North)
         : d == NorthEast ? shift_no_mask(shift_no_mask(bb, East), North)
         : d == SouthWest ? shift_no_mask(shift_no_mask(bb, West), South)
                          : shift_no_mask(shift_no_mask(bb, East), South);
}

constexpr Square lsb(Bitboard bb) {
  assert(bb);
  return Square(std::countr_zero(bb));
}
constexpr Square msb(Bitboard bb) {
  assert(bb);
  return Square(63 - std::countl_zero(bb));
}

constexpr bool more_than_one(Bitboard bb) { return bb & (bb - 1); }

constexpr uint32_t popcount(Bitboard bb) { return uint32_t(std::popcount(bb)); }

inline Square pop_lsb(Bitboard &bb) {
  assert(bb);
  Square s = lsb(bb);
  bb &= (bb - 1);
  return s;
}

/* Square funcs */
constexpr Square make_square(File f, Rank r) {
  return Square(int(f) + (8 * int(r)));
}

constexpr bool is_ok(Square s) { return s < NO_SQUARE; }

constexpr File file_of(Square s) {
  assert(is_ok(s));
  return File(int(s) & 7);
}
constexpr Rank rank_of(Square s) {
  assert(is_ok(s));
  return Rank((int(s) >> 3) & 7);
}

#define INC_DEC_OP(T)                                                          \
  inline T &operator++(T &t) { return t = T(int(t) + 1); }                     \
  inline T &operator--(T &t) { return t = T(int(t) - 1); }

inline Square &operator++(Square &s) {
  return s = is_ok(s) ? Square(int(s) + 1) : s;
}
inline Square &operator--(Square &s) {
  return s = is_ok(s) ? Square(int(s) - 1) : s;
}

INC_DEC_OP(File);
INC_DEC_OP(Rank);

/* Color funcs */
constexpr Color operator~(Color c) { return Color(c ^ Black); }

constexpr Rank relative_to(Rank r, Color c) {
  return c == White ? r : Rank(7 - int(r));
}
constexpr Square relative_to(Square s, Color c) {
  return c == White ? s : Square(int(s) ^ 56);
}

// Returns the *capturable* rank for this color pawn.
constexpr Rank ep_rank(Color c) { return c == White ? Rank_3 : Rank_6; }

/* Piece funcs */
inline Piece operator+(PieceT type, Color c) {
  assert(type != ALL_TYPES);
  return Piece(type, c);
}

/* CastlePerms funcs */
constexpr Square rook_from(CastlePerms cp) {
  return cp == WhiteKingside    ? H1
         : cp == WhiteQueenside ? A1
         : cp == BlackKingside  ? H8
                                : A8;
}
constexpr Square rook_to(CastlePerms cp) {
  return cp == WhiteKingside    ? F1
         : cp == WhiteQueenside ? D1
         : cp == BlackKingside  ? F8
                                : D8;
}
constexpr Square king_to(CastlePerms cp) {
  return cp == WhiteKingside    ? G1
         : cp == WhiteQueenside ? C1
         : cp == BlackKingside  ? G8
                                : C8;
}

constexpr CastlePerms kingside(Color c) {
  return c == White ? WhiteKingside : BlackKingside;
}
constexpr CastlePerms queenside(Color c) {
  return c == White ? WhiteQueenside : BlackQueenside;
}
