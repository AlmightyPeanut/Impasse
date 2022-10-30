//
// Created by Benedikt Hornig on 15.09.22.
//

#include "BoardState.h"

BoardState::BoardState(const Bitboard singles, const Bitboard doubles, const Bitboard occupiedWhite, const Bitboard occupiedBlack, const Color turn) {
    this->singles = singles;
    this->doubles = doubles;
    this->occupied[WHITE] = occupiedWhite;
    this->occupied[BLACK] = occupiedBlack;
    this->turn = turn;
}

bool BoardState::operator==(const BoardState &boardState) const {
    return singles == boardState.singles && doubles == boardState.doubles && occupied[WHITE] == boardState.occupied[WHITE]
        && occupied[BLACK] == boardState.occupied[BLACK] && turn == boardState.turn;
}
