//
// Created by Benedikt Hornig on 29.09.22.
//

#pragma once

#include "../Move.h"
#include "../Board.h"
#include "ZobristHash.h"
#include <map>
#include <array>

enum ValueFlag {
    EXACT,
    LOWER_BOUND,
    UPPER_BOUND
};

struct TTEntry {
    TTEntry();

    ~TTEntry() = default;

    TTEntry(Hash hash, Move move, int value, ValueFlag flag, int depth);

    HashKey hashKey;
    int value;
    ValueFlag valueFlag;
    Move bestMove;
    int searchDepth;
};


class TT {
private:
    std::map<HashKey, std::array<TTEntry, 2>> lookupTable{};

public:
    TT() = default;

    ~TT() = default;

    TTEntry lookupEntry(Hash hashValue);

    void store(const Board &board, Hash hashValue, Move move, int value, ValueFlag flag, int depth);

    void clearTable();
};
