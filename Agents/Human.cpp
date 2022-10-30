//
// Created by Benedikt Hornig on 07.10.22.
//

#include "Human.h"
#include <iostream>

// get a legal move from the user input and return the parsed move
Move Human::getMove(Board state) {
    std::cout << "Legal Moves: ";
    std::vector<Move> legalMoves = state.getLegalMoves();
    for (const auto &move: legalMoves) {
        std::cout << std::string(move) << ", ";
    }
    std::cout << std::endl;

    Move move;
    while (true) {
        std::cout << "Type your move: " << std::flush;
        std::string moveStr;
        std::cin >> moveStr;

        move = Move::fromNotation(moveStr);
        if (moveStr == "undo") {
            move = UNDO_MOVE;
            break;
        } else if (std::find(legalMoves.begin(), legalMoves.end(), move) != legalMoves.end())
            break;
        else
            std::cout << "This is not a valid move!" << std::endl;
    }

    return move;
}
