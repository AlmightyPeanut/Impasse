//
// Created by Benedikt Hornig on 15.09.22.
//

#include "RandomAgent.h"
#include <random>

// random move selection (only used for testing)
Move RandomAgent::getMove(Board state) {
    std::random_device rand = std::random_device();
    std::vector<Move> moves = state.getLegalMoves();
    return moves[rand() % moves.size()];
}
