#include <iostream>
#include <string>

#include "bitboard.h"
#include "position.h"
#include "types.h"
#include "uci.h"

int main(int argc, char *argv[]) {
  (void)argc, (void)argv;

  initialize();

  Position p;
  std::string mov;

  std::cout << "Hello!" << std::endl;

  while (true) {
    std::cout << p << std::endl;

    std::cout << "Move: ";
    std::getline(std::cin, mov);
    Move m = parse_move(mov, p);
    p.do_move(m);
  }

  return 0;
}
