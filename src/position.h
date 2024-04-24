/* -*- mode: c++; -*- */
#pragma once
#include "types.h"

class State {
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

    State *state;

    public:
        Position(std::string fen = "rnbkqbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKQBNR w KQkq - 0 1");

        bool inCheck() const;

        Square ep() const;

        int moves() const;
        int ply() const;

};
