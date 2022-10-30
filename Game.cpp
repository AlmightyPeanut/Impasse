//
// Created by Benedikt Hornig on 15.09.22.
//

#include <iostream>
#include "Game.h"

// 10 Minute game
auto PLAYER_MAX_TIME = std::chrono::milliseconds(1000 * 600);

Game::Game() {
    timeWhite = PLAYER_MAX_TIME;
    timeBlack = PLAYER_MAX_TIME;
    this->board = Board();
    moveStart = std::chrono::high_resolution_clock::now();
}

void Game::step(const Move& move) {
    // update players clocks
    auto moveTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - moveStart);
    if (board.getTurn() == WHITE) {
        timeWhite = timeWhite - moveTime;
    } else {
        timeBlack = timeBlack - moveTime;
    }

    if (move == UNDO_MOVE) {
        board.undoMove();
        board.undoMove();
        moveHistory.erase(moveHistory.end() - 2, moveHistory.end());
    } else {
        board.makeMove(move);
        moveHistory.push_back(move);
    }

    // start new move timer
    moveStart = std::chrono::high_resolution_clock::now();
}

bool Game::isOver() {
    return board.isOver();
}

Color Game::getResult() {
    return board.getResult();
}

void Game::visualizeBoard() {
    // display player clocks and the current board
    auto secondsLeftWhite = std::chrono::duration_cast<std::chrono::seconds>(timeWhite).count();
    std::cout << "White: " << secondsLeftWhite / 60 << ":";
    auto secondsWhite = secondsLeftWhite - secondsLeftWhite / 60 * 60;
    if (secondsWhite < 10) {
        std::cout << "0";
    }
    std::cout << secondsWhite << std::endl;

    auto secondsLeftBlack = std::chrono::duration_cast<std::chrono::seconds>(timeBlack).count();
    std::cout << "Black: " << secondsLeftBlack / 60 << ":";
    auto secondsBlack = secondsLeftBlack - secondsLeftBlack / 60 * 60;
    if (secondsBlack < 10) {
        std::cout << "0";
    }
    std::cout << secondsBlack << std::endl;

    board.visualizeBoard(WHITE);
}

std::string Game::getPGN() {
    std::string res;
    for (int i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0) {
            res += std::to_string(i / 2 + 1);
            res += ". ";
        }
        res += std::string(moveHistory[i]);
        res += " ";
    }

    return res;
}

std::string Game::getBoardNotation() {
    return board.getBoardNotation();
}

bool Game::getTurn() {
    return board.getTurn();
}

Board Game::getBoard() {
    return board;
}

void Game::setBoardByNotation(const std::string& notation) {
    board.setBoardByNotation(notation);
}

int Game::getMoveCount() {
    return (int) moveHistory.size();
}
