//
// Created by Benedikt Hornig on 08.10.22.
//

#include "evaluate.h"
#include <random>

const int WIN_VALUE = 1000;
const int MAX_RAND_WINDOW = 1;

std::random_device generator;
std::uniform_int_distribution<int> uniformDist(-MAX_RAND_WINDOW, MAX_RAND_WINDOW);

/*GameStage getGameStage(const Board& board) {
    int whitePieceCount = popCount(board.getSingles(WHITE) & board.getDoubles(WHITE));
    int blackPieceCount = popCount(board.getSingles(BLACK) & board.getDoubles(BLACK));

    if (whitePieceCount < 7 || blackPieceCount < 7)
        return GameStage::END_GAME;
    if (whitePieceCount < 12 || blackPieceCount < 12)
        // one piece was removed
        return GameStage::MIDDLE_GAME;
    return GameStage::END_GAME;
}*/

int pieceDifference(const Board& state) {
    // return the piece difference from the view of the given color (fewer pieces is better)
    // each piece is valued as 1 (doubles as 2 of course)
    int whitePieceCount = popCount(state.getSingles(WHITE)) + 2 * popCount(state.getDoubles(WHITE));
    int blackPieceCount = popCount(state.getSingles(BLACK)) + 2 * popCount(state.getDoubles(BLACK));

    return state.getTurn() == BLACK ? whitePieceCount - blackPieceCount : blackPieceCount - whitePieceCount;
}

int distanceSinglesLastRank(const Board& state) {
    // get value on how far all singles are away from the last rank
    Bitboard singles = state.getSingles(state.getTurn());

    int distanceWhite = 0;
    distanceWhite += popCount(singles & BB_RANK_1);
    distanceWhite += 2 * popCount(singles & BB_RANK_2);
    distanceWhite += 3 * popCount(singles & BB_RANK_3);
    distanceWhite += 4 * popCount(singles & BB_RANK_4);
    distanceWhite += 5 * popCount(singles & BB_RANK_5);
    distanceWhite += 6 * popCount(singles & BB_RANK_6);
    distanceWhite += 7 * popCount(singles & BB_RANK_7);
    distanceWhite += 8 * popCount(singles & BB_RANK_8);

    int distanceBlack = 0;
    distanceBlack += popCount(singles & BB_RANK_8);
    distanceBlack += 2 * popCount(singles & BB_RANK_7);
    distanceBlack += 3 * popCount(singles & BB_RANK_6);
    distanceBlack += 4 * popCount(singles & BB_RANK_5);
    distanceBlack += 5 * popCount(singles & BB_RANK_4);
    distanceBlack += 6 * popCount(singles & BB_RANK_3);
    distanceBlack += 7 * popCount(singles & BB_RANK_2);
    distanceBlack += 8 * popCount(singles & BB_RANK_1);

    return state.getTurn() == WHITE ? distanceBlack - distanceWhite : distanceWhite - distanceBlack;
}

int distanceFurthestSingleLastRank(const Board& state) {
    // get value on how far the furthest singles is away from the last rank
    Bitboard singles = state.getSingles(state.getTurn());

    int whiteDistance = 0;
    if (singles & BB_RANK_8)
        whiteDistance =  8;
    else if (singles & BB_RANK_7)
        whiteDistance =  7;
    else if (singles & BB_RANK_6)
        whiteDistance =  6;
    else if (singles & BB_RANK_5)
        whiteDistance =  5;
    else if (singles & BB_RANK_4)
        whiteDistance =  4;
    else if (singles & BB_RANK_3)
        whiteDistance =  3;
    else if (singles & BB_RANK_2)
        whiteDistance =  2;
    else if (singles & BB_RANK_1)
        whiteDistance =  1;

    int blackDistance =  0;
    if (singles & BB_RANK_1)
        blackDistance =  8;
    else if (singles & BB_RANK_2)
        blackDistance =  7;
    else if (singles & BB_RANK_3)
        blackDistance =  6;
    else if (singles & BB_RANK_4)
        blackDistance =  5;
    else if (singles & BB_RANK_5)
        blackDistance =  4;
    else if (singles & BB_RANK_6)
        blackDistance =  3;
    else if (singles & BB_RANK_7)
        blackDistance =  2;
    else if (singles & BB_RANK_8)
        blackDistance =  1;

    return state.getTurn() == WHITE ? blackDistance - whiteDistance : whiteDistance - blackDistance;
}

int distanceDoublesFirstRank(const Board& state) {
    // get value on how far all doubles are away from the first rank
    Bitboard doubles = state.getDoubles(state.getTurn());

    int distanceBlack = 0;
    distanceBlack += popCount(doubles & BB_RANK_1);
    distanceBlack += 2 * popCount(doubles & BB_RANK_2);
    distanceBlack += 3 * popCount(doubles & BB_RANK_3);
    distanceBlack += 4 * popCount(doubles & BB_RANK_4);
    distanceBlack += 5 * popCount(doubles & BB_RANK_5);
    distanceBlack += 6 * popCount(doubles & BB_RANK_6);
    distanceBlack += 7 * popCount(doubles & BB_RANK_7);

    int distanceWhite = 0;
    distanceWhite += popCount(doubles & BB_RANK_8);
    distanceWhite += 2 * popCount(doubles & BB_RANK_7);
    distanceWhite += 3 * popCount(doubles & BB_RANK_6);
    distanceWhite += 4 * popCount(doubles & BB_RANK_5);
    distanceWhite += 5 * popCount(doubles & BB_RANK_4);
    distanceWhite += 6 * popCount(doubles & BB_RANK_3);
    distanceWhite += 7 * popCount(doubles & BB_RANK_2);

    return state.getTurn() == WHITE ? distanceBlack - distanceWhite : distanceWhite - distanceBlack;
}

int distanceNearestDoubleFirstRank(const Board& state) {
    // get value on how far the nearest double is away from the first rank
    Bitboard doubles = state.getDoubles(state.getTurn());

    int blackDistance = 0;
    if (doubles & BB_RANK_7)
        blackDistance =  7;
    else if (doubles & BB_RANK_6)
        blackDistance =  6;
    else if (doubles & BB_RANK_5)
        blackDistance =  5;
    else if (doubles & BB_RANK_4)
        blackDistance =  4;
    else if (doubles & BB_RANK_3)
        blackDistance =  3;
    else if (doubles & BB_RANK_2)
        blackDistance =  2;
    else if (doubles & BB_RANK_1)
        blackDistance =  1;

    int whiteDistance =  0;
    if (doubles & BB_RANK_2)
        whiteDistance =  7;
    else if (doubles & BB_RANK_3)
        whiteDistance =  6;
    else if (doubles & BB_RANK_4)
        whiteDistance =  5;
    else if (doubles & BB_RANK_5)
        whiteDistance =  4;
    else if (doubles & BB_RANK_6)
        whiteDistance =  3;
    else if (doubles & BB_RANK_7)
        whiteDistance =  2;
    else if (doubles & BB_RANK_8)
        whiteDistance =  1;

    return state.getTurn() == WHITE ? blackDistance - whiteDistance : whiteDistance - blackDistance;
}

int doubleOnLastRank(const Board& state) {
    // double on the last rank is a potential previous promotion
    if (state.getDoubles(state.getTurn()) & (state.getTurn()==WHITE ? BB_RANK_8 : BB_RANK_1))
        return 1;
    return 0;
}

int singleOnFirstRank(const Board& state) {
    // single on the first rank is a potential previous bear off
    if (state.getSingles(state.getTurn()) & (state.getTurn()==WHITE ? BB_RANK_1 : BB_RANK_8))
        return 1;
    return 0;
}

int singleOnLastRank(const Board& state) {
    // single on the last rank is a potential piece for crowning
    if (state.getSingles(state.getTurn()) & (state.getTurn()==WHITE ? BB_RANK_8 : BB_RANK_1))
        return 1;
    return 0;
}

int isImpasse(const Board& state, const std::vector<Move>& moves) {
    int value = 0;

    for (const auto &move: moves) {
        if (move.isRemoving())
            return 1;
    }

    return value;
}

int canCrown(const Board& state, const std::vector<Move>& moves) {
    int value = 0;

    for (const auto &move: moves) {
        if (move.isCrowning())
            return 1;
    }

    return value;
}

int differenceDoublesSingles(const Board& state) {
    // having one double is better than having two singles in the end game especially
    return popCount(state.getDoubles(state.getTurn())) - popCount(state.getSingles(state.getTurn()));
}

int evaluate(Board& state, const evalConfig& config) {
    double value = 0;
    std::vector<Move> legalMoves = state.getLegalMoves();

    if (state.isOver()) {
        // Game is only over when it's the loser's turn
        return -WIN_VALUE;
    } else {
        int availableMoves = (int) legalMoves.size();

        value += config.pieceDifference * pieceDifference(state);
        value += config.distanceSinglesLastRank * distanceSinglesLastRank(state);
        value += config.distanceFurthestSingleLastRank * distanceFurthestSingleLastRank(state);
        value += config.distanceDoublesFirstRank * distanceDoublesFirstRank(state);
        value += config.distanceNearestDoubleFirstRank * distanceNearestDoubleFirstRank(state);
        value += config.doubleOnLastRank * doubleOnLastRank(state);
        value += config.singleOnFirstRank * singleOnFirstRank(state);
        value += config.singleOnLastRank * singleOnLastRank(state);
        value += config.isImpasse * isImpasse(state, legalMoves);
        value += config.canCrown * canCrown(state, legalMoves);
        value += config.differenceDoublesSingles * differenceDoublesSingles(state);
        value += config.availableMoves * availableMoves;
        value += config.sideToMove;

        value += uniformDist(generator);
    }

    // bound value function to be smaller than the win value
    // mostly used because of TD learning
    if (value > WIN_VALUE * 0.9) {
        return WIN_VALUE * 0.9;
    } else if (value < -WIN_VALUE * 0.9) {
        return -WIN_VALUE * 0.9;
    }

    return (int) round(value);
}

std::vector<double> evalGradByWeight(Board& state, const evalConfig& config) {
    // return sigmoid gradients for each weight
    std::vector<double> res;
    std::vector<Move> legalMoves = state.getLegalMoves();
    int availableMoves = (int) legalMoves.size();

    if (state.isOver()) {
        double value = sigmoidGrad(-WIN_VALUE);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
        res.push_back(value);
    } else {
        res.push_back(sigmoidGrad(config.pieceDifference * pieceDifference(state)));
        res.push_back(sigmoidGrad(config.distanceSinglesLastRank * distanceSinglesLastRank(state)));
        res.push_back(sigmoidGrad(config.distanceFurthestSingleLastRank * distanceFurthestSingleLastRank(state)));
        res.push_back(sigmoidGrad(config.distanceDoublesFirstRank * distanceDoublesFirstRank(state)));
        res.push_back(sigmoidGrad(config.distanceNearestDoubleFirstRank * distanceNearestDoubleFirstRank(state)));
        res.push_back(sigmoidGrad(config.doubleOnLastRank * doubleOnLastRank(state)));
        res.push_back(sigmoidGrad(config.singleOnFirstRank * singleOnFirstRank(state)));
        res.push_back(sigmoidGrad(config.singleOnLastRank * singleOnLastRank(state)));
        res.push_back(sigmoidGrad(config.isImpasse * isImpasse(state, legalMoves)));
        res.push_back(sigmoidGrad(config.canCrown * canCrown(state, legalMoves)));
        res.push_back(sigmoidGrad(config.differenceDoublesSingles * differenceDoublesSingles(state)));
        res.push_back(sigmoidGrad(config.availableMoves * availableMoves));
        res.push_back(sigmoidGrad(config.sideToMove));
    }

    return res;
}

evalConfig::operator std::string() const {
    std::string res;

    res += std::to_string(pieceDifference) + ", ";
    res += std::to_string(distanceSinglesLastRank) + ", ";
    res += std::to_string(distanceFurthestSingleLastRank) + ", ";
    res += std::to_string(distanceDoublesFirstRank) + ", ";
    res += std::to_string(distanceNearestDoubleFirstRank) + ", ";
    res += std::to_string(doubleOnLastRank) + ", ";
    res += std::to_string(singleOnFirstRank) + ", ";
    res += std::to_string(singleOnLastRank) + ", ";
    res += std::to_string(isImpasse) + ", ";
    res += std::to_string(canCrown) + ", ";
    res += std::to_string(differenceDoublesSingles) + ", ";
    res += std::to_string(availableMoves) + ", ";
    res += std::to_string(sideToMove) + ", ";
    return res;
}
