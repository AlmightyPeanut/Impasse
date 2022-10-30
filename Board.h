//
// Created by Benedikt Hornig on 05.09.22.
//

#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <array>
#include "Move.h"
#include "BoardState.h"

class Board {
private:
    Bitboard singles;
    Bitboard doubles;
    std::array<Bitboard, 2> occupied = {};

    Color turn;

    std::vector<Move> legalMoves{};

    std::vector<BoardState> boardHistory{};
    std::vector<Move> moveHistory{};

private:
    static void initializeGlobalVars();

    static std::tuple<std::vector<Bitboard>, std::vector<std::map<Bitboard, Bitboard>>> createMoveTable(const std::array<int, 2>& deltas);

    static Bitboard slidingMoves(Square square, Bitboard occupiedBoard, const std::array<int, 2>& deltas);

    [[nodiscard]] Bitboard getPieceToCrown() const;

public:
    Board();

    ~Board() = default;

    explicit Board(BoardState state);

    void visualizeBoard(Color perspective) const;

    void visualizeAllBitboards();

    [[nodiscard]] std::vector<Move> getLegalMoves();

    void makeMove(const Move& move);

    void undoMove();

    [[nodiscard]] bool isOver() const;

    std::string getBoardNotation();

    void setBoardByNotation(const std::string& notation);

    void clearBoard();

    BoardState getBoardState();

    void setBoardState(BoardState boardState);

    [[nodiscard]] Color getTurn() const;

    Color getResult();

    [[nodiscard]] Bitboard getSingles(Color color) const;

    [[nodiscard]] Bitboard getDoubles(Color color) const;

    static void visualizeBitboard(Bitboard bb);

    void setPieceAt(PieceType piece, Square square);

    int getPieceCount(Color color);
};
