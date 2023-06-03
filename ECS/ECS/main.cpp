#include <memory>
#include "Game.h"

int main(int argc, char* argv[]) {
    auto game = std::make_unique<Game>();

    if (!game->Initialize("SDL2 Window", 800, 600)) {
        return 1;
    }

    game->Run();

    return 0;
}
