//
// Created by Benedikt Hornig on 08.10.22.
//

#pragma once

#include "../Board.h"

struct evalConfig {
    double pieceDifference;
    double distanceSinglesLastRank;
    double distanceFurthestSingleLastRank;
    double distanceDoublesFirstRank;
    double distanceNearestDoubleFirstRank;
    double doubleOnLastRank;
    double singleOnFirstRank;
    double singleOnLastRank;
    double isImpasse;
    double canCrown;
    double differenceDoublesSingles;
    double availableMoves;
    double sideToMove;

    explicit operator std::string() const;
};

/*enum GameStage {
    OPENING,
    MIDDLE_GAME,
    END_GAME
};*/

int evaluate(Board& state, const evalConfig& config);
std::vector<double> evalGradByWeight(Board& state, const evalConfig& config);
