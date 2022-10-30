//
// Created by Benedikt Hornig on 07.10.22.
//

#pragma once

#include "BaseAgent.h"

class Human : public BaseAgent {
public:
    explicit Human(Color color) : BaseAgent(color){}

    Move getMove(Board state) override;
};
