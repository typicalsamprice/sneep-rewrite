/* -*- mode: c++; -*- */
#pragma once
#include "types.h"

class State {
public:
  Square enPassant;
  int halfmoves;

  Bitboard pinners[2];
  Bitboard blockers[2];
  Bitboard checkers;

  Piece captured;

  uint8_t castlingPerms; // 0x1 K, 0x2 Q, 0x4 k, 0x01 q

  State *prev;
};

class Position {
  Piece squares[64];

  Bitboard colorBB[2];
  Bitboard pieceBB[8]; // No king necessary, use ALL_TYPES instead

  int plies;
  Color toMove;

  State *state;

  void remove_piece(Square s);
  void put_piece(Square s, Piece p);
  void swap_piece(Square from, Square to);

  void update_state();
  void update_bcs(Color c);

public:
  Position(std::string fen =
               "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  Bitboard pieces(PieceT pt = ALL_TYPES) const;
  template <typename... PieceTypes>
  Bitboard pieces(PieceT pt, PieceTypes... pts) const;
  Bitboard pieces(Color c) const;
  template <typename... PieceTypes>
  Bitboard pieces(Color c, PieceTypes... pts) const;
  Square king(Color c) const;

  bool inCheck() const { return state->checkers; }

  Piece piece_on(Square s) const;
  bool empty(Square s) const { return (pieces() & bb_from(s)) == 0; }
  Square ep() const;

  int moves() const;
  int ply() const;

  Color to_move() const { return toMove; }

  Bitboard attacks_to(Square s) const { return attacks_to(s, pieces()); }
  Bitboard attacks_to(Square s, Bitboard occupied) const;

  Bitboard sliders_to(Square s, Bitboard occupied) const;

  bool is_legal(Move m) const;
  bool is_pseudo_legal(Move m) const;
  bool is_capture(Move m) const;

  void do_move(Move m);
  void undo_move(Move m);

  bool can_castle(CastlePerms cp) const {
    return (state->castlingPerms & int(cp)) > 0;
  }

  Bitboard blockers(Color c) const;
  Bitboard pinners(Color c) const;
  Bitboard checkers() const;
};

std::ostream &operator<<(std::ostream &os, const Position &pos);
