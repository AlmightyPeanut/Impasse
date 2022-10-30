//
// Created by Benedikt Hornig on 07.10.22.
//

#include <iostream>
#include <cmath>
#include <random>
#include "Game.h"
#include "Agents/AlphaBeta.h"

int NUM_CONFIG_TYPES = 13;

// constants for TD learning
double LEARNING_RATE = 0.1;
double DISCOUNT = 0.9;

// pseudo-random number generators
std::random_device randomDevice;
std::mt19937 randGen(randomDevice());
std::uniform_int_distribution<int> uniform100(0, 100);

// changes a config value by the given value
evalConfig changeConfig(evalConfig config, const int configValueToChange, const double value) {
    switch (configValueToChange % NUM_CONFIG_TYPES) {
        case 0:
            config.pieceDifference += value;
            break;
        case 1:
            config.distanceSinglesLastRank += value;
            break;
        case 2:
            config.distanceFurthestSingleLastRank += value;
            break;
        case 3:
            config.distanceDoublesFirstRank += value;
            break;
        case 4:
            config.distanceNearestDoubleFirstRank += value;
            break;
        case 5:
            config.doubleOnLastRank += value;
            break;
        case 6:
            config.singleOnFirstRank += value;
            break;
        case 7:
            config.singleOnLastRank += value;
            break;
        case 8:
            config.isImpasse += value;
            break;
        case 9:
            config.canCrown += value;
            break;
        case 10:
            config.differenceDoublesSingles += value;
            break;
        case 11:
            config.availableMoves += value;
            break;
        case 12:
            config.sideToMove += value;
            break;
        default:
            assert(false);
    }

    return config;
}

// run games with TD learning
evalConfig runTrial(AlphaBeta& white, evalConfig whiteConf, AlphaBeta& black, evalConfig blackConf, Color learner, int runs) {
    Game game;

    float whiteWins = 0, blackWins = 0;

    for (int i = 0; i < runs; ++i) {
        game = Game();
        white.reset(true, true);
        black.reset(true, true);

        white.setEvalConfig(whiteConf);
        black.setEvalConfig(blackConf);

        std::vector<Move> moves;
        Move move;

        std::vector<Board> gameStatesWhite, gameStatesBlack;
        std::vector<double> evalPredsWhite, evalPredsBlack;
        std::vector<std::vector<double>> derivWeightWhite, derivWeightBlack;

        int whiteMoves = 0;
        int blackMoves = 0;

        Board b = game.getBoard();
        if (learner == WHITE) {
            // track game state and evaluation values
            gameStatesWhite.push_back(b);
            evalPredsWhite.push_back(evaluate(b, whiteConf));
            derivWeightWhite.push_back(evalGradByWeight(b, whiteConf));
        }

        while (!game.isOver()) {
            move = game.getTurn() == WHITE ?
                        white.getMove(game.getBoard()) :
                        black.getMove(game.getBoard());

            if (uniform100(randGen) > 80) {
                // add randomness for self-play to converge
                moves = game.getBoard().getLegalMoves();
                move = moves[randGen() % moves.size()];
            }

            game.step(move);

            // track game state and evaluation values
            b = game.getBoard();
            if (game.getTurn() == WHITE && learner == WHITE) {
                whiteMoves++;
                gameStatesWhite.push_back(b);
                evalPredsWhite.push_back(sigmoid(evaluate(b, whiteConf)));
                derivWeightWhite.push_back(evalGradByWeight(b, whiteConf));
            } else if (game.getTurn() == BLACK && learner == BLACK) {
                blackMoves++;
                gameStatesBlack.push_back(b);
                evalPredsBlack.push_back(sigmoid(evaluate(b, blackConf)));
                derivWeightBlack.push_back(evalGradByWeight(b, blackConf));
            }

            // TD learning
            for (int weigthNum = 0; weigthNum < NUM_CONFIG_TYPES; ++weigthNum) {
                if (learner == WHITE) {
                    for (int t = 0; t < whiteMoves; ++t) {
                        double sum = 0;
                        for (int k = 1; k < t; ++k) {
                            sum += pow(DISCOUNT, t-k) * derivWeightWhite[k][weigthNum];
                        }
                        double weightDelta = LEARNING_RATE * (evalPredsWhite[t+1] - evalPredsWhite[t]) * sum;
                        whiteConf = changeConfig(whiteConf, weigthNum,  weightDelta);
                    }
                } else {
                    for (int t = 0; t < blackMoves-1; ++t) {
                        double sum = 0;
                        for (int k = 1; k < t; ++k) {
                            sum += pow(DISCOUNT, t-k) * derivWeightBlack[k][weigthNum];
                        }
                        double weightDelta = LEARNING_RATE * (evalPredsBlack[t+1] - evalPredsBlack[t]) * sum;
                        blackConf = changeConfig(blackConf, weigthNum, weightDelta);
                    }
                }
            }
        }

        // update last value as win or lose value
        int winVal = 1000;
        if (learner == WHITE) {
            if (game.getResult() == WHITE) {
                evalPredsWhite.back() = sigmoid(winVal);
                double value = sigmoidGrad(winVal);
                std::vector<double> res;
                for (int j = 0; j < NUM_CONFIG_TYPES; ++j)
                    res.push_back(value);
                derivWeightWhite.back() = res;

                whiteWins++;
            } else if (game.getResult() == BLACK) {
                evalPredsWhite.back() = sigmoid(-winVal);
                double value = sigmoidGrad(-winVal);
                std::vector<double> res;
                for (int j = 0; j < NUM_CONFIG_TYPES; ++j)
                    res.push_back(value);
                derivWeightWhite.back() = res;

                blackWins++;
            }
        } else if (learner == BLACK) {
            if (game.getResult() == WHITE) {
                evalPredsBlack.back() = sigmoid(-winVal);
                double value = sigmoidGrad(-winVal);
                std::vector<double> res;
                for (int j = 0; j < NUM_CONFIG_TYPES; ++j)
                    res.push_back(value);
                derivWeightBlack.back() = res;

                whiteWins++;
            } else if (game.getResult() == BLACK) {
                evalPredsBlack.back() = sigmoid(1000);
                double value = sigmoidGrad(1000);
                std::vector<double> res;
                for (int j = 0; j < NUM_CONFIG_TYPES; ++j)
                    res.push_back(value);
                derivWeightBlack.back() = res;

                blackWins++;
            }
        }

        // TD learning
        for (int weigthNum = 0; weigthNum < NUM_CONFIG_TYPES; ++weigthNum) {
            if (learner == WHITE) {
                for (int t = 0; t < whiteMoves; ++t) {
                    double sum = 0;
                    for (int k = 1; k < t; ++k) {
                        sum += pow(DISCOUNT, t-k) * derivWeightWhite[k][weigthNum];
                    }
                    double weightDelta = LEARNING_RATE * (evalPredsWhite[t+1] - evalPredsWhite[t]) * sum;
                    whiteConf = changeConfig(whiteConf, weigthNum,  weightDelta);
                }
            } else {
                for (int t = 0; t < blackMoves-1; ++t) {
                    double sum = 0;
                    for (int k = 1; k < t; ++k) {
                        sum += pow(DISCOUNT, t-k) * derivWeightBlack[k][weigthNum];
                    }
                    double weightDelta = LEARNING_RATE * (evalPredsBlack[t+1] - evalPredsBlack[t]) * sum;
                    blackConf = changeConfig(blackConf, weigthNum, weightDelta);
                }
            }
        }
    }

    return learner == WHITE ? whiteConf : blackConf;
}

int main () {
    // manual configs
    evalConfig whiteConfig{50, 1, 1, 1, 1, 7, 7, 3, 10, 5, -2, 1, 1};
    evalConfig blackConfig{20, 1, 1, 1, 1, 7, 7, 3, 10, 5, -2, 1, 1};

    evalConfig testWhite{};
    evalConfig testBlack{};

    AlphaBeta white = AlphaBeta(WHITE, false);
    AlphaBeta black = AlphaBeta(BLACK, false);

    // runs against the current evaluation config
    int runs = 5;

    for (int i = 0; i < 100000; ++i) {
        testWhite = whiteConfig;
        testBlack = blackConfig;
        whiteConfig = runTrial(white, whiteConfig, black, testWhite, WHITE, runs);
        blackConfig = runTrial(white, testBlack, black, blackConfig, BLACK, runs);

        std::cout << "\nWhite config: " << std::endl;
        std::cout << std::string(whiteConfig) << std::endl;
        std::cout << "Black config: " << std::endl;
        std::cout << std::string(blackConfig) << std::endl;
    }

    std::cout << "\nWhite config: " << std::endl;
    std::cout << std::string(whiteConfig) << std::endl;
    std::cout << "Black config: " << std::endl;
    std::cout << std::string(blackConfig) << std::endl;

    return 0;
}
