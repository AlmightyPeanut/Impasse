//
// Created by Benedikt Hornig on 19.09.22.
//

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "ZobristHash.h"

void validateGame(const std::filesystem::directory_entry &path) {
    ZobristHash hash = ZobristHash();
    Board oldBoard;

    Board board = Board();
    Hash hashValue = hash.initialHash(board);

    std::string line;
    std::ifstream gameFile (path);
    bool isBoardStr = true;

    while (getline(gameFile, line)) {
        if (isBoardStr) {
            std::string boardStr = board.getBoardNotation();
            if(boardStr != line) {
                throw std::runtime_error("Board notation in file " + std::string(path.path()) + " has failed");
            }
        } else {
            Move move = Move::fromNotation(line);
            std::string moveStr = std::string(move);
            if(moveStr != line) {
                throw std::runtime_error("Move notation in file " + std::string(path.path()) + " has failed");
            }

            BoardState bs1 = board.getBoardState();
            board.makeMove(move);

            // test undo move
            board.undoMove();
            BoardState bs2 = board.getBoardState();
            if(bs1 != bs2) {
                throw std::runtime_error("Undo operation in file " + std::string(path.path()) + " has failed");
            }
            board.makeMove(move);

            hashValue = hash.continuousHash(hashValue, oldBoard, move);
            Hash initHash = hash.initialHash(board);
            if(hashValue != initHash) {
                throw std::runtime_error("Hash generation in file " + std::string(path.path()) + " has failed");
            }

            oldBoard = board;
        }

        isBoardStr = !isBoardStr;
    }
}

int main() {
    // validate game moves with games played with a random Ludii agent
    std::string gamesDir = "../Game Files/";
    int gameNum = 0;
    for (const auto &entry: std::filesystem::directory_iterator(gamesDir)) {
        if (entry.path().string() == "../Game Files/.DS_Store")
            continue;

        gameNum++;
        if (gameNum % 1000 == 0)
            std::cout << "### Game number: " << gameNum << std::endl;

        try {
            validateGame(entry);
        } catch (std::runtime_error& error) {
            std::cout << error.what() << std::endl;
        }
    }

    std::cout << "All games validated" << std::endl;
    return 0;
}
