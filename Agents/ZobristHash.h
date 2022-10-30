//
// Created by Benedikt Hornig on 29.09.22.
//

#pragma once

#include <array>
#include "../Board.h"

typedef u_int64_t Hash;
typedef u_int32_t HashKey;

class ZobristHash {
private:
    // 4 pieces (S, D, s, d) with a random number for each square
    std::array<std::array<Hash, 64>, 4> randNumberTable{};
    std::array<Hash , 2> sideToMoveHash{};

    void initTable();

public:
    ZobristHash();

    ~ZobristHash() = default;

    Hash initialHash(const Board& board);

    Hash continuousHash(Hash hash, const Board& board, Move move);
};
