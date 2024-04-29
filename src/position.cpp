#include "position.h"
#include "bitboard.h"
#include <cstring>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Position& pos) {
  std::string sep = "+---+---+---+---+---+---+---+---+\n";
  os << sep;
  for (Rank r = Rank_8; r >= Rank_1; --r) {
    for (File f = File_A; f <= File_H; ++f) {
      Square s = make_square(f, r);
      if (pos.empty(s))
        os << "|   ";
      else {
        Piece p = pos.piece_on(s);
        char ptc = " pnbrqk?"[p.type];
        ptc = p.color == White ? ptc - 32 : ptc;
        os << "| " << ptc << ' ';
      }
    }
    os << "| " << std::to_string(1 + r) << "\n" << sep;
  }
  os << "  a   b   c   d   e   f   g   h\n";
  return os;
}

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

Square Position::king(Color c) const {
  return lsb(pieces(c, King));
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

    Color us = to_move();
    Color them = ~us;

    State *newSt = new State;
    std::memcpy(newSt, state, sizeof(State));
    newSt->prev = state;
    state = newSt;

    state->halfmoves++;

    assert(movp.color == us);
    assert(movp.type != NO_TYPE && movp.type != ALL_TYPES);

    MoveT flag = m.flag;
    PieceT prom = m.prom;

    Square f = m.from;
    Square t = m.to;

    assert(f != t);
    assert(is_ok(f));
    assert(is_ok(t));

    Piece captured = piece_on(t);
    assert(captured.type != ALL_TYPES);
    if (flag == EnPassant) {
        assert(captured.type == NO_TYPE);
        captured = Piece(Pawn, them);
    }

    if (captured.type != NO_TYPE) {
        Square cap = flag == EnPassant ? make_square(file_of(t), rank_of(f)) : t;
        Piece rem = piece_on(cap);
        assert(rem == captured);

        state->captured = rem;

        if (rem.type == Rook) {
          if (file_of(cap) == File_A && can_castle(queenside(them)))
            state->castlingPerms &= ~queenside(them);
          else if (file_of(cap) == File_H && can_castle(kingside(them)))
            state->castlingPerms &= ~kingside(them);
        }

        state->halfmoves = 0;
    }

    if (flag == Castle) {
      assert(movp.type == King);
      assert(f == relative_to(E1, us));
      assert(distance(f, t) == 2);
      assert(rank_of(f) == rank_of(t));
      assert(rank_of(f) == relative_to(Rank_1, us));

      CastlePerms cp = f > t ? queenside(us) : kingside(us);
      assert(can_castle(cp));

      Square rk_f = rook_from(cp);
      Square rk_t = rook_to(cp);

      assert(king_to(cp) == t);
      assert(piece_on(rk_f) == Piece(Rook, us));
      assert(empty(rk_t));

      remove_piece(f);
      put_piece(t, Piece(King, us));
      remove_piece(rk_f);
      put_piece(rk_t, Piece(Rook, us));

      state->castlingPerms &= ~cp;
    } else {
      remove_piece(f);
      put_piece(t, movp);
    }

    state->enPassant = NO_SQUARE;

    if (movp.type == Pawn) {
      state->halfmoves = 0;
      Square possible_ep = make_square(file_of(f), relative_to(Rank_3, us));
      if (distance(f, t) == 2
          && (attacks(possible_ep, Piece(Pawn, us)) & pieces(them, Pawn)))
        state->enPassant = possible_ep;
      else if (flag == Promotion) {
        assert(rank_of(t) == relative_to(Rank_8, us));
        remove_piece(t);
        put_piece(t, Piece(prom, us));
      }
    } else if (movp.type == King) {
      state->castlingPerms &= ~(kingside(us) | queenside(us));
    } else if (movp.type == Rook) {
      if (file_of(f) == File_A && can_castle(queenside(us)))
        state->castlingPerms &= ~queenside(us);
      else if (file_of(f) == File_H && can_castle(kingside(us)))
        state->castlingPerms &= ~kingside(us);
    }

    toMove = ~toMove;
    plies++;
    update_state();
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

void Position::update_state() {
  Color us = to_move();
  Color them = ~us;
  Square k = king(us);

  state->checkers = attacks_to(k) & pieces(them);
  update_bcs(White);
  update_bcs(Black);
}

void Position::update_bcs(Color us) {
  Square k = king(us);
  Bitboard raw_attacks = sliders_to(k, 0) & pieces(~us);
  while (raw_attacks) {
    Square slider = pop_lsb(raw_attacks);
    Bitboard bw = between(k, slider) & pieces();
    if (bw && !more_than_one(bw)) {
      // Not a checking slider
      state->pinners[~us] |= bb_from(slider);
      if (bw & pieces(us))
        state->blockers[us] |= bw;
    }
  }
}
