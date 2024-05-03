#include "movegen.h"
#include "position.h"
#include "types.h"

int score(const Position &pos) {
  return popcount(pos.pieces(White)) - popcount(pos.pieces(Black));
}

int search(Position &pos, int depth, int alpha, int beta, Move &mov) {
  if (depth == 0)
    return alpha;
  for (Move m : generate(pos)) {
    pos.do_move(m);
    int score = -search(pos, depth - 1, -beta, -alpha, mov);
    pos.undo_move(m);
    if (score >= beta)
      return beta;
    if (score > alpha) {
      mov = m;
      return alpha;
    }
  }
  return alpha;
}
