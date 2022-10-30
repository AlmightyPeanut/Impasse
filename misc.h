//
// Created by Benedikt Hornig on 07.09.22.
//

#pragma once

#include <cstdint>
#include <array>

typedef uint64_t Bitboard;
typedef int Square;
typedef int Color;

extern Color BLACK;
extern Color WHITE;
extern Color NO_COLOR;

extern const std::array<int, 64> SQUARES;

const extern Bitboard BB_EMPTY;
const extern Bitboard BB_A1, BB_B1, BB_C1, BB_D1, BB_E1, BB_F1, BB_G1, BB_H1;
const extern Bitboard BB_A2, BB_B2, BB_C2, BB_D2, BB_E2, BB_F2, BB_G2, BB_H2;
const extern Bitboard BB_A3, BB_B3, BB_C3, BB_D3, BB_E3, BB_F3, BB_G3, BB_H3;
const extern Bitboard BB_A4, BB_B4, BB_C4, BB_D4, BB_E4, BB_F4, BB_G4, BB_H4;
const extern Bitboard BB_A5, BB_B5, BB_C5, BB_D5, BB_E5, BB_F5, BB_G5, BB_H5;
const extern Bitboard BB_A6, BB_B6, BB_C6, BB_D6, BB_E6, BB_F6, BB_G6, BB_H6;
const extern Bitboard BB_A7, BB_B7, BB_C7, BB_D7, BB_E7, BB_F7, BB_G7, BB_H7;
const extern Bitboard BB_A8, BB_B8, BB_C8, BB_D8, BB_E8, BB_F8, BB_G8, BB_H8;

const extern std::array<Bitboard, 64> BB_SQUARES;

extern const Bitboard BB_RANK_1;
extern const Bitboard BB_RANK_2;
extern const Bitboard BB_RANK_3;
extern const Bitboard BB_RANK_4;
extern const Bitboard BB_RANK_5;
extern const Bitboard BB_RANK_6;
extern const Bitboard BB_RANK_7;
extern const Bitboard BB_RANK_8;

enum PieceType {
    WHITE_SINGLE,
    WHITE_DOUBLE,
    BLACK_SINGLE,
    BLACK_DOUBLE
};

int mostSignificantBit(Bitboard bb);

int squareFile(Square square);

int squareRank(Square square);

int squareDistance(Square sq1, Square sq2);

bool squareOnSameDiagonal (Square sq1, Square sq2);

std::vector<Bitboard> getSubsets(Bitboard mask);

std::vector<Square> scanReversed(Bitboard bb);

double sigmoid(double v);

double sigmoidGrad(double v);

int popCount(Bitboard b);
