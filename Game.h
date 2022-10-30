//
// Created by Benedikt Hornig on 15.09.22.
//

#pragma once

#include <chrono>
#include "Board.h"
#include "Agents/BaseAgent.h"

class Game {
private:
    std::chrono::high_resolution_clock::time_point moveStart;
    std::chrono::high_resolution_clock::duration timeWhite{};
    std::chrono::high_resolution_clock::duration timeBlack{};

    Board board;
    std::vector<Move> moveHistory{};
public:
    Game();

    ~Game() = default;

    void step(const Move& move);

    bool isOver();

    Color getResult();

    void visualizeBoard();

    std::string getPGN();

    std::string getBoardNotation();

    bool getTurn();

    Board getBoard();

    void setBoardByNotation(const std::string& notation);

    int getMoveCount();
};
