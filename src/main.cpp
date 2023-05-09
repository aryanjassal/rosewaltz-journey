#include "game.h"

static int WIDTH = 1366;
static int HEIGHT = 768;

Game *RosewaltzJourney;

int main() {
  RosewaltzJourney = new Game(WIDTH, HEIGHT);
  RosewaltzJourney->init();
  RosewaltzJourney->run();

  return 0;
}