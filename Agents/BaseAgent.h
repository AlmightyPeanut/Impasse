//
// Created by Benedikt Hornig on 15.09.22.
//

#pragma once

#include "../Move.h"
#include "../Board.h"

// parent class for all agents
class BaseAgent {
protected:
    Color color;
public:
    explicit BaseAgent(Color color) {
        this->color = color;
    }

    ~BaseAgent() = default;

    void reset(){};

    virtual Move getMove(Board state) = 0;
};
