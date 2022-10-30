#include <iostream>
#include <fstream>
#include <random>
#include "Game.h"
#include "Agents/AlphaBeta.h"
#include "Agents/Human.h"

int main() {
    // main game loop for playing one game (human vs agent or agent vs agent)
    auto abWHITE = AlphaBeta(WHITE);
    auto abBLACK = AlphaBeta(BLACK);
    auto human = Human(BLACK);

    evalConfig baseLine{50, 1, 1, 1, 1, 7, 7, 3, 10, 5, -2, 1, 1};
    abWHITE.setEvalConfig(baseLine);
    abBLACK.setEvalConfig(baseLine);

    Game game;
    game = Game();
    abWHITE.reset();
    abBLACK.reset();

    int gameStep = 0;
    while (!game.isOver()) {
        game.visualizeBoard();
        Move move = game.getTurn() == WHITE ?
                    abWHITE.getMove(game.getBoard()) :
                    // human.getMove(game.getBoard());
                    abBLACK.getMove(game.getBoard());
        std::cout << "Chosen Move: " << std::string(move) << std::endl;

        game.step(move);
        gameStep++;
    }

    game.visualizeBoard();
    std::cout << "Game steps: " << gameStep << std::endl;

    if (game.getResult() == WHITE)
        std::cout << "\nWhite wins!" << std::endl;
    else
        std::cout << "\nBlack wins!" << std::endl;

    return 0;
}
