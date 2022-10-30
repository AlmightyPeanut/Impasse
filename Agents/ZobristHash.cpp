//
// Created by Benedikt Hornig on 29.09.22.
//

#include "ZobristHash.h"
#include <random>

void ZobristHash::initTable() {
    std::random_device r;
    std::mt19937_64 generator(r());

    std::vector<Hash> randNumbers;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 64; ++j) {
            Hash number = generator();
            // don't allow duplicate random numbers
            while (std::find(randNumbers.begin(), randNumbers.end(), number) != randNumbers.end()) {
                number = generator();
            }
            randNumberTable[i][j] = number;
            randNumbers.push_back(number);
        }
    }

    sideToMoveHash[WHITE] = generator();
    sideToMoveHash[BLACK] = generator();
}

Hash ZobristHash::initialHash(const Board& board) {
    Hash value = 0;
    for (const auto &square: scanReversed(board.getSingles(WHITE))) {
        value ^= randNumberTable[0][square];
    }

    for (const auto &square: scanReversed(board.getDoubles(WHITE))) {
        value ^= randNumberTable[1][square];
    }

    for (const auto &square: scanReversed(board.getSingles(BLACK))) {
        value ^= randNumberTable[2][square];
    }

    for (const auto &square: scanReversed(board.getDoubles(BLACK))) {
        value ^= randNumberTable[3][square];
    }

    return value ^ sideToMoveHash[board.getTurn()];
}

Hash ZobristHash::continuousHash(Hash hash, const Board& board, Move move) {

    // add new move to hash
    int singleIdx = board.getTurn() == WHITE ? 0 : 2;
    int doubleIdx = singleIdx + 1;

    if (move.isRemoving()) {
        // Impasse
        if (board.getSingles(board.getTurn()) & BB_SQUARES[move.getFromSquare()]) {
            hash ^= randNumberTable[singleIdx][move.getFromSquare()];
        } else {
            hash ^= randNumberTable[doubleIdx][move.getFromSquare()];
            hash ^= randNumberTable[singleIdx][move.getFromSquare()];
        }
    } else {
        if (board.getSingles(board.getTurn()) & BB_SQUARES[move.getFromSquare()]) {
            // single moves
            hash ^= randNumberTable[singleIdx][move.getFromSquare()];
            hash ^= randNumberTable[singleIdx][move.getToSquare()];
        } else if (board.getDoubles(board.getTurn()) & BB_SQUARES[move.getFromSquare()]) {
            // double moves
            hash ^= randNumberTable[doubleIdx][move.getFromSquare()];

            if (board.getSingles(board.getTurn()) & BB_SQUARES[move.getToSquare()]) {
                // transpose
                hash ^= randNumberTable[singleIdx][move.getFromSquare()];
                hash ^= randNumberTable[singleIdx][move.getToSquare()];
                hash ^= randNumberTable[doubleIdx][move.getToSquare()];
            } else {
                hash ^= randNumberTable[doubleIdx][move.getToSquare()];
            }

            if (board.getTurn() == WHITE && BB_RANK_1 & BB_SQUARES[move.getToSquare()]
                || board.getTurn() == BLACK && BB_RANK_8 & BB_SQUARES[move.getToSquare()]) {
                // bear off
                hash ^= randNumberTable[doubleIdx][move.getToSquare()];
                hash ^= randNumberTable[singleIdx][move.getToSquare()];
            }
        }
    }

    // crowning moves
    if (move.isCrowning()) {
        hash ^= randNumberTable[singleIdx][move.getPieceToCrown()];
        hash ^= randNumberTable[doubleIdx][move.getPieceToCrown()];
        hash ^= randNumberTable[singleIdx][move.getCrowningPiece()];
    }

    // apply turn hash value
    return hash ^ sideToMoveHash[WHITE] ^ sideToMoveHash[BLACK];
}

ZobristHash::ZobristHash() {
    initTable();
}

