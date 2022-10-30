//
// Created by Benedikt Hornig on 07.09.22.
//

#include "Move.h"
#include <array>

std::array<char, 8> FILE_NAMES = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
std::array<char, 8> RANK_NAMES = {'1', '2', '3', '4', '5', '6', '7', '8'};

Move::Move() {
    this->fromSquare = -1;
    this->toSquare = -1;
    this->removing = false;
    this->crowningPiece = -1;
    this->pieceToCrown = -1;
}

Move::Move(const Square fromSquare, const Square toSquare, const bool isRemoving) {
    this->fromSquare = fromSquare;
    this->toSquare = toSquare;
    this->removing = isRemoving;
    this->crowningPiece = -1;
    this->pieceToCrown = -1;
}

Move::Move(const Square fromSquare, const Square toSquare, const bool isRemoving, const Square crowningPiece, const Square pieceToCrown) {
    this->fromSquare = fromSquare;
    this->toSquare = toSquare;
    this->removing = isRemoving;
    this->crowningPiece = crowningPiece;
    this->pieceToCrown = pieceToCrown;
}

Move::operator std::string() const {
    // parse move object to string
    std::string res;
    res += FILE_NAMES[squareFile(fromSquare)];
    res += RANK_NAMES[squareRank(fromSquare)];

    if (isRemoving()) {
        res += 'x';
    } else {
        if (fromSquare != toSquare) {
            res += FILE_NAMES[squareFile(toSquare)];
            res += RANK_NAMES[squareRank(toSquare)];
        }
    }

    if (isCrowning()) {
        res += '^';
        res += FILE_NAMES[squareFile(crowningPiece)];
        res += RANK_NAMES[squareRank(crowningPiece)];
        res += FILE_NAMES[squareFile(pieceToCrown)];
        res += RANK_NAMES[squareRank(pieceToCrown)];
    }

    return res;
}

Move Move::fromNotation(const std::string& notation) {
    // parse move from a FEN-like string
    long fileIdx = -1, rankIdx = -1;
    bool isCrowning = false;
    bool isRemoving = false;

    Square fromSq = -1, toSq = -1, crownP = -1, pToCrown = -1;

    for (const auto &item: notation) {
        auto fileName = std::find(FILE_NAMES.begin(), FILE_NAMES.end(), item);
        if (fileName != std::end(FILE_NAMES)) {
            fileIdx = std::distance(FILE_NAMES.begin(), fileName);
        } else {
            auto rankName = std::find(RANK_NAMES.begin(), RANK_NAMES.end(), item);
            if (rankName != std::end(RANK_NAMES))
                rankIdx = std::distance(RANK_NAMES.begin(), rankName);
            else if (item == '^')
                isCrowning = true;
            else if (item == 'x') {
                isRemoving = true;
                continue;
            }

            if (fileIdx != -1 && rankIdx != -1) {
                auto sq = (int) (rankIdx * 8 + fileIdx);
                if (isCrowning)
                    if (crownP == -1)
                        crownP = sq;
                    else
                        pToCrown = sq;
                else if (fromSq != -1)
                    toSq = sq;
                else
                    fromSq = sq;

                fileIdx = -1;
                rankIdx = -1;
            }
        }
    }

     if (isCrowning)
        return {fromSq, toSq, isRemoving, crownP, pToCrown};

    return {fromSq, toSq, isRemoving};
}

bool Move::operator==(const Move move) const {
    return move.fromSquare == fromSquare && move.toSquare == toSquare && move.removing == removing && move.crowningPiece == crowningPiece && move.pieceToCrown == pieceToCrown;
}

bool Move::isRemoving() const {
    return removing;
}

Square Move::getFromSquare() const {
    return fromSquare;
}

Square Move::getToSquare() const {
    return toSquare;
}

bool Move::isCrowning() const {
    return pieceToCrown != -1;
}

Square Move::getPieceToCrown() const {
    return pieceToCrown;
}

Square Move::getCrowningPiece() const {
    return crowningPiece;
}
