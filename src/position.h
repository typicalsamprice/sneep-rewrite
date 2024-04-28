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

  uint8_t castlingPerms; // 0x1 K, 0x2 Q, 0x4 k, 0x01 q
};

class Position {
  Piece squares[64];

  Bitboard colorBB[2];
  Bitboard pieceBB[7]; // No king necessary, use ALL_TYPES instead
  Square kings[2];

  int plies;
  Color to_move;

  State *state;

public:
  Position(std::string fen =
               "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w KQkq - 0 1");

  Bitboard pieces(PieceT pt = ALL_TYPES) const;
  template <typename... PieceTypes>
  Bitboard pieces(PieceT pt, PieceTypes... pts) const;
  Bitboard pieces(Color c) const;
  template <typename... PieceTypes>
  Bitboard pieces(Color c, PieceTypes... pts) const;

  bool inCheck() const { return state->checkers; }

  Piece piece_on(Square s) const;
  bool empty(Square s) const { return piece_on(s).type == NO_TYPE; }
  Square ep() const;

  int moves() const;
  int ply() const;

  Bitboard attacks_to(Square s) const { return attacks_to(s, pieces()); }
  Bitboard attacks_to(Square s, Bitboard occupied) const;

  Bitboard sliders_to(Square s, Bitboard occupied) const;

  bool is_legal(Move m) const;
  bool is_pseudo_legal(Move m) const;
  bool is_capture(Move m) const;

  void do_move(Move m);
  void undo_move(Move m);
};
