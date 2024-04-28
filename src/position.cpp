#include "position.h"
#include "bitboard.h"

Position::Position(std::string fen) {}

Bitboard Position::pieces(PieceT pt) const { return pieceBB[pt]; }

template <typename... PieceTypes>
Bitboard Position::pieces(PieceT pt, PieceTypes... pts) const {
  return pieces(pt) | pieces(pts...);
}

Bitboard Position::pieces(Color c) const { return colorBB[c]; }

template <typename... PieceTypes>
Bitboard Position::pieces(Color c, PieceTypes... pts) const {
  return pieces(c) & pieces(pts...);
}

Piece Position::piece_on(Square s) const {
  assert(is_ok(s));
  return squares[s];
}
Square Position::ep() const { return state->enPassant; }

int Position::moves() const { return 1 + (ply() - to_move()) / 2; }
int Position::ply() const { return plies; }

Bitboard Position::attacks_to(Square s, Bitboard occupied) const {
  Bitboard attackers = 0;
  for (Piece p : {Piece(Pawn, White), Piece(Pawn, Black), Piece(Knight, White),
                  Piece(King, White)}) {
    Bitboard atts_from_target = attacks(s, p, occupied);
    if (p.type == Pawn)
      // Only the other color can attack that way - it's reversed.
      atts_from_target &= pieces(~p.color, p.type);
    else
      atts_from_target &= pieces(p.type);
    attackers |= atts_from_target;
  }
  attackers |= sliders_to(s, occupied);
  return attackers;
}

Bitboard Position::sliders_to(Square s, Bitboard occupied) const {
  Bitboard rv = 0;
  for (Piece p : {Piece(Bishop, White), Piece(Rook, White)}) {
    Bitboard potented = attacks(s, p, occupied);
    rv |= potented & pieces(p.type, Queen);
  }
  return rv;
}

bool Position::is_legal(Move m) const { return true; }

// Hope to god we don't generate non-PL moves.
// I am out of time to implement that
bool Position::is_pseudo_legal(Move m) const { return true; }

bool Position::is_capture(Move m) const { return !empty(m.to); }

void Position::do_move(Move m) {
    assert(!empty(m.from));
    Piece movp = piece_on(m.from);

    MoveT flag = m.flag;
    PieceT prom = m.prom;

    Square f = m.from;
    Square t = m.to;
}

void Position::undo_move(Move m) {}

// PRIV METHODS
void Position::remove_piece(Square s) {
    assert(!empty(s));
    Piece p = piece_on(s);
    Bitboard bb = bb_from(s);
    squares[s] = Piece();
    pieceBB[p.type] ^= bb;
    pieceBB[ALL_TYPES] ^= bb;
    colorBB[p.color] ^= bb;
}

void Position::put_piece(Square s, Piece p) {
    assert(empty(s));
    Bitboard bb = bb_from(s);
    squares[s] = p;
    pieceBB[p.type] |= bb;
    pieceBB[ALL_TYPES] |= bb;
    colorBB[p.color] |= bb;
}

void Position::swap_piece(Square from, Square to) {
    assert(empty(to));
    assert(!empty(from));
    Piece p = piece_on(from);
    // Can't call it xor for "accessibility" reasons or some garbage
    Bitboard xr = bb_from(from) | bb_from(to);
    pieceBB[p.type] ^= xr;
    pieceBB[ALL_TYPES] ^= xr;
    colorBB[p.color] ^= xr;
}
