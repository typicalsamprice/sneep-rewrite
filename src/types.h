/* -*- mode: c++; -*- */
#pragma once
#include <cstdint>
#include <cassert>
#include <bit>

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
  North = 8,
  South = -North,
  East = 1,
  West = -East,
  NorthEast = North + East,
  NorthWest = North + West,
  SouthEast = South + East,
  SouthWest = South + West
};

enum Color { White, Black };

typedef uint64_t Bitboard;

/* BB funcs */

constexpr Bitboard bb_from(Square s) {
    assert(s != NO_SQUARE);
    return (Bitboard(1) << int(s));
}
constexpr Bitboard bb_from(File f) {
    return Bitboard(0x0101010101010101 << int(f));
}
constexpr Bitboard bb_from(Rank r) {
    return Bitboard(0xff) << (8 * int(r));
}

template<Direction D>
constexpr Bitboard shift(Bitboard bb) {
    return D == North ? bb << 8
        : D == South ? bb >> 8
        : D == East ? ((bb << 1) &~ bb_from(File_A))
        : D == West ? ((bb >> 1) &~ bb_from(File_H))
        : D == NorthWest ? shift<North>(shift<West>(bb))
        : D == NorthEast ? shift<North>(shift<East>(bb))
        : D == SouthWest ? shift<South>(shift<West>(bb))
        : shift<South>(shift<East>(bb));
}
template<Direction D>
constexpr Bitboard shift_n(Bitboard bb, int iter) {
    return iter == 0 ? bb : shift_n<D>(shift<D>(bb), iter - 1);
}

constexpr Square lsb(Bitboard bb) {
    assert(bb);
    return Square(std::countr_zero(bb));
}
constexpr Square msb(Bitboard bb) {
  assert(bb);
  return Square(std::countl_zero(bb));
}

constexpr bool more_than_one(Bitboard bb) {
  return bb & (bb - 1);
}

/* Square funcs */
constexpr Square operator+(File f, Rank r) {
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

/* Color funcs */
constexpr Color operator~(Color c) {
  return Color(c ^ Black);
}
