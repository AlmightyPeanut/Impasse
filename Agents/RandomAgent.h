//
// Created by Benedikt Hornig on 15.09.22.
//

#pragma once

#include "BaseAgent.h"

class RandomAgent : public BaseAgent {
public:
    explicit RandomAgent(Color color) : BaseAgent(color){}

    Move getMove(Board state) override;
};
