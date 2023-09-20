#include "game.h"

int main() {
  Game game(1280, 720, "Rosewaltz Journey", false);
  game.set_active();
  game.run();
  return 0;
}
