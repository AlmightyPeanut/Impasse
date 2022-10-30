//
// Created by Benedikt Hornig on 15.09.22.
//

#pragma once

#include "misc.h"

struct BoardState {
    Bitboard singles;
    Bitboard doubles;
    Bitboard occupied[2]{};
    Color turn;

    ~BoardState() = default;

    BoardState(Bitboard singles, Bitboard doubles, Bitboard occupiedWhite, Bitboard occupiedBlack, Color turn);

    bool operator == (const BoardState& boardState) const;
};
