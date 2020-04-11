#include "game.h"
#include "common.h"

int main() {
    Game game("Snake3D", Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    game.run();

    return 0;
}
