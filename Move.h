//
// Created by Benedikt Hornig on 07.09.22.
//

#pragma once

#include <string>
#include "misc.h"

class Move {
private:
    Square fromSquare;
    Square toSquare;
    bool removing;
    // crownPiece is the piece that is being crowned
    Square crowningPiece;
    Square pieceToCrown;

public:
    [[nodiscard]] Square getCrowningPiece() const;

    [[nodiscard]] Square getPieceToCrown() const;

    [[nodiscard]] Square getFromSquare() const;

    [[nodiscard]] Square getToSquare() const;

    [[nodiscard]] bool isRemoving() const;

    [[nodiscard]] bool isCrowning() const;

    Move();

    ~Move() = default;

    Move(Square fromSquare, Square toSquare, bool isRemoving);

    Move(Square fromSquare, Square toSquare, bool isRemoving, Square crowningPiece, Square pieceToCrown);

    explicit operator std::string() const;

    bool operator == (Move move) const;

    bool operator < (Move move) const {
        return fromSquare < move.fromSquare || toSquare < move.toSquare || removing < move.removing || crowningPiece < move.crowningPiece || pieceToCrown < move.pieceToCrown;
    }

    static Move fromNotation(const std::string& notation);

};

const Move NONE_MOVE = Move();
const Move UNDO_MOVE = Move(-1, -1 , true);
