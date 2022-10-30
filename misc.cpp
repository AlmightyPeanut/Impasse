//
// Created by Benedikt Hornig on 07.09.22.
//

#include <algorithm>
#include <cmath>
#include <vector>
#include "misc.h"

Color BLACK = 0;
Color WHITE = 1;
Color NO_COLOR = 2;

const std::array<int, 64> SQUARES = {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31,
        32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47,
        48, 49, 50, 51, 52, 53, 54, 55,
        56, 57, 58, 59, 60, 61, 62, 63,
};

const Bitboard BB_EMPTY = 0;
const Bitboard BB_A1=1<<0, BB_B1=1<<1, BB_C1=1<<2, BB_D1=1<<3, BB_E1=1<<4, BB_F1=1<<5, BB_G1=1<<6, BB_H1=1<<7;
const Bitboard BB_A2=1<<8, BB_B2=1<<9, BB_C2=1<<10, BB_D2=1<<11, BB_E2=1<<12, BB_F2=1<<13, BB_G2=1<<14, BB_H2=1<<15;
const Bitboard BB_A3=1<<16, BB_B3=1<<17, BB_C3=1<<18, BB_D3=1<<19, BB_E3=1<<20, BB_F3=1<<21, BB_G3=1<<22, BB_H3=1<<23;
const Bitboard BB_A4=1<<24, BB_B4=1<<25, BB_C4=1<<26, BB_D4=1<<27, BB_E4=1<<28, BB_F4=1<<29, BB_G4=1<<30, BB_H4=(long) 1<<31;
const Bitboard BB_A5=(long)1<<32, BB_B5=(long)1<<33, BB_C5=(long)1<<34, BB_D5=(long)1<<35, BB_E5=(long)1<<36, BB_F5=(long)1<<37, BB_G5=(long)1<<38, BB_H5=(long)1<<39;
const Bitboard BB_A6=(long)1<<40, BB_B6=(long)1<<41, BB_C6=(long)1<<42, BB_D6=(long)1<<43, BB_E6=(long)1<<44, BB_F6=(long)1<<45, BB_G6=(long)1<<46, BB_H6=(long)1<<47;
const Bitboard BB_A7=(long)1<<48, BB_B7=(long)1<<49, BB_C7=(long)1<<50, BB_D7=(long)1<<51, BB_E7=(long)1<<52, BB_F7=(long)1<<53, BB_G7=(long)1<<54, BB_H7=(long)1<<55;
const Bitboard BB_A8=(long)1<<56, BB_B8=(long)1<<57, BB_C8=(long)1<<58, BB_D8=(long)1<<59, BB_E8=(long)1<<60, BB_F8=(long)1<<61, BB_G8=(long)1<<62, BB_H8=(unsigned long) 1<<63;

const std::array<Bitboard, 64> BB_SQUARES = {
        BB_A1, BB_B1, BB_C1, BB_D1, BB_E1, BB_F1, BB_G1, BB_H1,
        BB_A2, BB_B2,BB_C2,BB_D2,BB_E2,BB_F2,BB_G2,BB_H2,
        BB_A3,BB_B3,BB_C3,BB_D3,BB_E3,BB_F3,BB_G3,BB_H3,
        BB_A4,BB_B4,BB_C4,BB_D4,BB_E4,BB_F4,BB_G4,BB_H4,
        BB_A5,BB_B5,BB_C5,BB_D5,BB_E5,BB_F5,BB_G5,BB_H5,
        BB_A6,BB_B6,BB_C6,BB_D6,BB_E6,BB_F6,BB_G6,BB_H6,
        BB_A7,BB_B7,BB_C7,BB_D7,BB_E7,BB_F7,BB_G7,BB_H7,
        BB_A8,BB_B8,BB_C8,BB_D8,BB_E8,BB_F8,BB_G8,BB_H8,
};

const Bitboard BB_RANK_1 = BB_A1 | BB_B1 | BB_C1 | BB_D1 | BB_E1 | BB_F1 | BB_G1 | BB_H1;
const Bitboard BB_RANK_2 = BB_A2 | BB_B2 | BB_C2 | BB_D2 | BB_E2 | BB_F2 | BB_G2 | BB_H2;
const Bitboard BB_RANK_3 = BB_A3 | BB_B3 | BB_C3 | BB_D3 | BB_E3 | BB_F3 | BB_G3 | BB_H3;
const Bitboard BB_RANK_4 = BB_A4 | BB_B4 | BB_C4 | BB_D4 | BB_E4 | BB_F4 | BB_G4 | BB_H4;
const Bitboard BB_RANK_5 = BB_A5 | BB_B5 | BB_C5 | BB_D5 | BB_E5 | BB_F5 | BB_G5 | BB_H5;
const Bitboard BB_RANK_6 = BB_A6 | BB_B6 | BB_C6 | BB_D6 | BB_E6 | BB_F6 | BB_G6 | BB_H6;
const Bitboard BB_RANK_7 = BB_A7 | BB_B7 | BB_C7 | BB_D7 | BB_E7 | BB_F7 | BB_G7 | BB_H7;
const Bitboard BB_RANK_8 = BB_A8 | BB_B8 | BB_C8 | BB_D8 | BB_E8 | BB_F8 | BB_G8 | BB_H8;

int mostSignificantBit(const Bitboard bb) {
    // get the most significant (most left) bit of a number
    return (int) floor(log2(bb));
}

int squareFile(const Square square) {
    // return the file of the given square
    return (int) (square & 7);
}

int squareRank(const Square square) {
    // return the rank of the given square
    return (int) (square >> 3);
}

int squareDistance(const Square sq1, const Square sq2) {
    // calc the distance between squares. Diagonal distance is counted as 1
    return (int) std::max(
            std::abs(squareFile(sq1) - squareFile(sq2)),
            std::abs(squareRank(sq1) - squareRank(sq2))
    );
}

bool squareOnSameDiagonal (const Square sq1, const Square sq2) {
    // https://www.chessprogramming.org/Diagonals
    return ((sq2 - sq1) % 9) == 0;
}

std::vector<Bitboard> getSubsets(const Bitboard mask) {
    // return all subsets of the given mask
    // https://www.chessprogramming.org/Traversing_Subsets_of_a_Set

    std::vector<Bitboard> res;
    Bitboard subset = BB_EMPTY;

    do {
        res.push_back(subset);
        subset = (subset - mask) & mask;
    } while (subset);

    return res;
}

std::vector<Square> scanReversed(Bitboard bb){
    // return an array which consists of each square represented in the given bitboard in reverse order
    std::vector<Square> res;
    int r;
    while (bb) {
        r = mostSignificantBit(bb);
        res.push_back(r);
        bb ^= BB_SQUARES[r];
    }

    return res;
}

double sigmoid(double v) {
    return 1 / (1 + exp(-v));
}

double sigmoidGrad(double v) {
    return sigmoid(v) * (1 - sigmoid(v));
}

int popCount(Bitboard b) {
    // Get pop count of given bitstring
    // https://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
    int count;
    for (count = 0; b; ++count) {
        b &= b - 1;
    }

    return count;
}
