//
// Created by Benedikt Hornig on 05.09.22.
//

#include <bitset>
#include <iostream>
#include <array>
#include "Board.h"

bool initializedGlobalVars = false;
std::vector<Bitboard> BB_FORWARD_SLIDES_MASKS, BB_BACKWARD_SLIDES_MASKS;
std::vector<std::map<Bitboard, Bitboard>> BB_FORWARD_SLIDES_MOVES, BB_BACKWARD_SLIDES_MOVES;

Board::Board(){
    initializeGlobalVars();

    singles = BB_A1 | BB_A7 | BB_D2 | BB_D8 | BB_E1 | BB_E7 | BB_H2 | BB_H8;
    doubles = BB_B2 | BB_B8 | BB_C1 | BB_C7 | BB_F2 | BB_F8 | BB_G1 | BB_G7;
    occupied[WHITE] = BB_A1 | BB_B8 | BB_C7 | BB_D2 | BB_E1 | BB_F8 | BB_G7 | BB_H2;
    occupied[BLACK] = BB_A7 | BB_B2 | BB_C1 | BB_D8 | BB_E7 | BB_F2 | BB_G1 | BB_H8;

    turn = WHITE;
}

Board::Board(BoardState state) {
    initializeGlobalVars();

    singles = state.singles;
    doubles = state.doubles;
    occupied[WHITE] = state.occupied[WHITE];
    occupied[BLACK] = state.occupied[BLACK];
    turn = state.turn;
}

void Board::visualizeAllBitboards() {
    // only for debugging
    printf("white\n");
    visualizeBitboard(occupied[WHITE]);
    printf("black\n");
    visualizeBitboard(occupied[BLACK]);
    printf("singles\n");
    visualizeBitboard(singles);
    printf("doubles\n");
    visualizeBitboard(doubles);
}

void Board::visualizeBitboard(const Bitboard bb){
    // helper function to visualize the bitboards
    std::bitset<64> bs(bb);
    std::string boardString;
    std::string fileString;

    int i = 0;
    for (const auto &bit: bs.to_string()){
        fileString = "  " + fileString;
        fileString = bit + fileString;
        i++;
        if (i%8 == 0) {
            boardString.append(fileString + "\n");
            fileString = "";
        }
    }

    std::cout << boardString + "\n";
}

void Board::visualizeBoard(const Color perspective) const{
    // print the board to the console from the perspective given

    printf("\n----------------------");

    Square square;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (j % 8 == 0){
                printf("\n");
            }

            if (perspective){
                square = (56 - 8 * i) + j;
            } else {
                square = i * 8 + 7 - j;
            }

            Bitboard bb_square = BB_SQUARES[square];

            if (bb_square & singles)
                (bb_square & occupied[WHITE]) ? printf("S  ") : printf("s  ");
            else if (bb_square & doubles)
                (bb_square & occupied[WHITE]) ? printf("D  ") : printf("d  ");
            else
                printf(".  ");

        }
    }
    printf("\n----------------------\n");
}

Bitboard Board::slidingMoves(const Square square, const Bitboard occupiedBoard, const std::array<int, 2>& deltas) {
    // return a move mask representing possible sliding moves from a square given occupancies on a board and the move directions
    /* Example
     * Given occupiedBoard
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  1  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0

     * result
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
    0  0  0  1  0  0  0  0
    0  0  1  0  0  0  0  0
    0  1  0  0  0  0  0  0
    0  0  0  0  0  0  0  0
     */

    Bitboard res = BB_EMPTY;

    for (const auto &delta: deltas) {
        Square sq = square;
        while (true) {
            sq += delta;
            if (!(0 <= sq && sq < 64) || squareDistance(sq, sq - delta) > 2 || occupiedBoard & BB_SQUARES[sq]) break;

            res |= BB_SQUARES[sq];
        }
    }

    return res;
}

std::tuple<std::vector<Bitboard>, std::vector<std::map<Bitboard, Bitboard>>> Board::createMoveTable(const std::array<int, 2>& deltas){
    // create a move table for each square and each possible configuration of blocking pieces

    std::vector<Bitboard> maskTable;
    std::vector<std::map<Bitboard, Bitboard>> moveTable;

    for (const auto &square: SQUARES){
        if (squareRank(square) % 2 != squareFile(square) % 2) {
            // skip white tiles
            maskTable.emplace_back();
            moveTable.emplace_back();
            continue;
        }

        std::map<Bitboard, Bitboard> moves;
        Bitboard mask = slidingMoves(square, BB_EMPTY, deltas);

        for (const auto &subset: getSubsets(mask)) {
            // subset represents a possible configuration of blocking pieces
            moves[subset] = slidingMoves(square, subset, deltas);
        }

        maskTable.push_back(mask);
        moveTable.push_back(moves);
    }

    return {maskTable, moveTable};
}

void Board::initializeGlobalVars(){
    if (initializedGlobalVars) return;

    // initialize slide masks and moves
    std::tuple<std::vector<Bitboard>, std::vector<std::map<Bitboard, Bitboard>>> maskMoveTable;

    const std::array<int, 2> forwardMoves = {7, 9};
    maskMoveTable = createMoveTable(forwardMoves);
    BB_FORWARD_SLIDES_MASKS = get<0>(maskMoveTable);
    BB_FORWARD_SLIDES_MOVES = get<1>(maskMoveTable);

    const std::array<int, 2> backwardMoves = {-9, -7};
    maskMoveTable = createMoveTable(backwardMoves);
    BB_BACKWARD_SLIDES_MASKS = get<0>(maskMoveTable);
    BB_BACKWARD_SLIDES_MOVES = get<1>(maskMoveTable);

    initializedGlobalVars = true;
}

Bitboard Board::getPieceToCrown() const {
    // returns a piece (as bitboard) if there is a piece waiting to be crowned
    Bitboard pieceToCrown = singles & occupied[turn];
    pieceToCrown &= turn ? BB_RANK_8 : BB_RANK_1;

    return pieceToCrown;
}

// Move Generator
std::vector<Move> Board::getLegalMoves() {
    if (isOver())
        return {};

    // return if already generated
    if (!legalMoves.empty())
        return legalMoves;

    // Basic moves //
    for (const auto &fromSquare: scanReversed(singles & occupied[turn])){
        // single slide
        Bitboard forwardMoves;
        if (turn){
            Bitboard blockMask = BB_FORWARD_SLIDES_MASKS[fromSquare] & (occupied[WHITE] | occupied[BLACK]);
            forwardMoves = BB_FORWARD_SLIDES_MOVES[fromSquare][blockMask];
        } else {
            Bitboard blockMask = BB_BACKWARD_SLIDES_MASKS[fromSquare] & (occupied[WHITE] | occupied[BLACK]);
            forwardMoves = BB_BACKWARD_SLIDES_MOVES[fromSquare][blockMask];
        }
        for (const auto &toSquare: scanReversed(forwardMoves)) {
            Bitboard otherSingles = singles & occupied[turn] ^ BB_SQUARES[fromSquare];
            if ((turn == WHITE ? (BB_SQUARES[toSquare] & BB_RANK_8) : (BB_SQUARES[toSquare] & BB_RANK_1)) && otherSingles) {
                // crown moves, if there is another single available
                for (const auto &crownPiece: scanReversed(otherSingles)) {
                    legalMoves.emplace_back(fromSquare, toSquare, false, crownPiece, toSquare);
                }
            } else {
                // normal slide moves
                legalMoves.emplace_back(fromSquare, toSquare, false);
            }
        }
    }

    // double slide
    for (const auto &fromSquare: scanReversed(doubles & occupied[turn])) {
        Bitboard backwardMoves;
        if (turn)
            backwardMoves = BB_BACKWARD_SLIDES_MOVES[fromSquare][BB_BACKWARD_SLIDES_MASKS[fromSquare] & (occupied[WHITE] | occupied[BLACK])];
        else
            backwardMoves = BB_FORWARD_SLIDES_MOVES[fromSquare][BB_FORWARD_SLIDES_MASKS[fromSquare] & (occupied[WHITE] | occupied[BLACK])];
        for (const auto &toSquare: scanReversed(backwardMoves)) {
            if ((turn == WHITE ? (BB_SQUARES[toSquare] & BB_RANK_1) : (BB_SQUARES[toSquare] & BB_RANK_8)) && getPieceToCrown())
                // crowning after bear off is possible
                legalMoves.emplace_back(fromSquare, toSquare, false, toSquare, mostSignificantBit(getPieceToCrown()));
            else
                legalMoves.emplace_back(fromSquare, toSquare, false);
        }

        // transpose
        //If you have a single adjacent to one of your doubles,
        // and in a nearer row than the double, you can take
        // the top checker of that double and transfer it onto the single.
        // A transpose can only be made backward (toward the owner), not forward.
        Square toSquare = turn ? fromSquare - 7 : fromSquare + 7;
        if (toSquare >= 0 && toSquare < 64 && (BB_SQUARES[toSquare] & singles & occupied[turn])){
            Bitboard crowningSingles = (singles & occupied[turn]) ^ BB_SQUARES[toSquare];
            if ((turn == WHITE ? BB_SQUARES[fromSquare] & BB_RANK_8 : BB_SQUARES[fromSquare] & BB_RANK_1) && crowningSingles)
                // crowning after transpose is possible
                for (const auto &crowningPiece: scanReversed(crowningSingles)) {
                    legalMoves.emplace_back(fromSquare, toSquare, false, crowningPiece, fromSquare);
                }
            else
                legalMoves.emplace_back(fromSquare, toSquare, false);
        }

        toSquare = turn ? fromSquare - 9 : fromSquare + 9;
        if (toSquare >= 0 && toSquare < 64 && (BB_SQUARES[toSquare] & singles & occupied[turn])){
            Bitboard crowningSingles = (singles & occupied[turn]) ^ BB_SQUARES[toSquare];
            if ((turn == WHITE ? BB_SQUARES[fromSquare] & BB_RANK_8 : BB_SQUARES[fromSquare] & BB_RANK_1) && crowningSingles)
                // crowning after transpose is possible
                for (const auto &crowningPiece: scanReversed(crowningSingles)) {
                    legalMoves.emplace_back(fromSquare, toSquare, false, crowningPiece, fromSquare);
                }
            else
                legalMoves.emplace_back(fromSquare, toSquare, false);
        }
    }

    // impasse
    if (legalMoves.empty()) {
        for (const auto &fromSquare: scanReversed(occupied[turn])) {
            if ((BB_SQUARES[fromSquare] & doubles) && getPieceToCrown()) {
                // crowning is possible after removing a piece of a double
                if ((turn && BB_SQUARES[fromSquare] & BB_RANK_8) || (!turn && BB_SQUARES[fromSquare] & BB_RANK_1))
                    // additional crowning move possible if two singles on furthest rank
                    legalMoves.emplace_back(fromSquare, -1, true, mostSignificantBit(getPieceToCrown()), fromSquare);
                legalMoves.emplace_back(fromSquare, -1, true, fromSquare, mostSignificantBit(getPieceToCrown()));
            } else
                legalMoves.emplace_back(fromSquare, -1, true);
        }
    }

    return legalMoves;
}

bool Board::isOver() const{
    // game ends if there are no pieces left for either side
    if (!occupied[WHITE] || !occupied[BLACK]) {
        return true;
    }

    return false;
}

void Board::makeMove(const Move& move) {
    // add state to board history
    boardHistory.push_back(getBoardState());
    moveHistory.push_back(move);

    Bitboard fromSquare = BB_SQUARES[move.getFromSquare()];
    Bitboard toSquare = BB_SQUARES[move.getToSquare()];

    if (move.isRemoving()) {
        // handle impasse moves
        if (singles & fromSquare) {
            singles ^= fromSquare;
            occupied[turn] ^= fromSquare;
        } else {
            doubles ^= fromSquare;
            singles |= fromSquare;
        }
    } else {
        // normal moves
        // check if it is a single or a double
        if (singles & fromSquare) {
            occupied[turn] ^= fromSquare;
            singles ^= fromSquare;

            // Do not allow a single to be moved on another single
            /*if (singles & toSquare) { // debug
                BoardState cur = getBoardState();
                for (int i = 0; i < boardHistory.size(); ++i) {
                    setBoardState(boardHistory[i]);
                    visualizeBoard(WHITE);
                    std::cout << std::string(moveHistory[i]) << std::endl;
                    visualizeAllBitboards();
                }
                setBoardState(cur);
                visualizeBoard(WHITE);
                visualizeAllBitboards();
            }*/
            assert(!(singles & toSquare));
            singles |= toSquare;
        } else if (doubles & fromSquare) {
            doubles ^= fromSquare;

            if (singles & toSquare) {
                // transpose
                singles |= fromSquare;
                singles ^= toSquare;
                doubles |= toSquare;
            } else {
                occupied[turn] ^= fromSquare;
                doubles |= toSquare;
            }

            // check for bear off moves and handle them automatically
            if ((turn == WHITE && BB_RANK_1 & toSquare) || (turn == BLACK && BB_RANK_8 & toSquare)) {
                doubles ^= toSquare;
                singles |= toSquare;
            }
        } else {
            /*BoardState cur = getBoardState(); // debug
            for (int i = 0; i < boardHistory.size(); ++i) {
                setBoardState(boardHistory[i]);
                visualizeBoard(WHITE);
                std::cout << std::string(moveHistory[i]) << std::endl;
                visualizeAllBitboards();
            }
            setBoardState(cur);
            visualizeBoard(WHITE);
            visualizeAllBitboards();*/
            assert(false);
        }

        occupied[turn] |= toSquare;
    }

    if (move.isCrowning()) {
        Bitboard pieceToCrown = BB_SQUARES[move.getPieceToCrown()];
        Bitboard crowningPiece = BB_SQUARES[move.getCrowningPiece()];
        singles ^= pieceToCrown | crowningPiece;
        doubles |= pieceToCrown;
        occupied[turn] ^= crowningPiece;
    }

    /*if ((singles ^ doubles ^ occupied[WHITE] ^ occupied[BLACK]) != BB_EMPTY // debug
        || (singles & doubles) != BB_EMPTY || (occupied[WHITE] & occupied[BLACK]) != BB_EMPTY) {
        BoardState cur = getBoardState();
        for (int i = 0; i < boardHistory.size(); ++i) {
            setBoardState(boardHistory[i]);
            visualizeBoard(WHITE);
            std::cout << std::string(moveHistory[i]) << std::endl;
            visualizeAllBitboards();
        }
        setBoardState(cur);
        visualizeBoard(WHITE);
        visualizeAllBitboards();
    }*/
    assert((singles ^ doubles ^ occupied[WHITE] ^ occupied[BLACK]) == BB_EMPTY);
    assert((singles & doubles) == BB_EMPTY);
    assert((occupied[WHITE] & occupied[BLACK]) == BB_EMPTY);

    turn = !turn;
    legalMoves.clear();
}

void Board::undoMove() {
    setBoardState(boardHistory.back());
    boardHistory.pop_back();
    moveHistory.pop_back();
}

std::string Board::getBoardNotation() {
    // return a notation similar to the FEN notation for chess
    std::string res;

    int skip = 0;
    for (int i = 0; i < 64; ++i) {
        if (skip == 8) {
            res += std::to_string(skip);
            skip = 0;
        }

        if (i > 0 && i % 8 == 0) {
            if (skip > 0) {
                res += std::to_string(skip);
                skip = 0;
            }
            res += "/";
        }

        if (BB_SQUARES[i] & occupied[WHITE]) {
            if (skip > 0) {
                res += std::to_string(skip);
                skip = 0;
            }
            if (BB_SQUARES[i] & singles)
                res += "S";
            else
                res += "D";
        } else if (BB_SQUARES[i] & occupied[BLACK]) {
            if (skip > 0) {
                res += std::to_string(skip);
                skip = 0;
            }
            if (BB_SQUARES[i] & singles)
                res += "s";
            else
                res += "d";
        } else {
            skip++;
        }
    }

    if (skip != 0)
        res += std::to_string(skip);

    res += " - ";
    res += turn ? "w" : "b";

    return res;
}

void Board::setBoardByNotation(const std::string& notation) {
    // set up the board like in the given FEN-like notation
    clearBoard();
    Square sq = 0;
    for (const auto &piece: notation) {
        switch (piece) {
            case 'S':
                singles |= BB_SQUARES[sq];
                occupied[WHITE] |= BB_SQUARES[sq];
                break;
            case 'D':
                doubles |= BB_SQUARES[sq];
                occupied[WHITE] |= BB_SQUARES[sq];
                break;
            case 's':
                singles |= BB_SQUARES[sq];
                occupied[BLACK] |= BB_SQUARES[sq];
                break;
            case 'd':
                doubles |= BB_SQUARES[sq];
                occupied[BLACK] |= BB_SQUARES[sq];
                break;
            case 'w':
                turn = WHITE;
                break;
            case 'b':
                turn = BLACK;
                break;
            case '/':
            case ' ':
            case '-':
                sq--;
                break;
            default:
                sq += piece - '0' - 1;
                break;
        }
        sq++;
    }
}

void Board::clearBoard() {
    singles = BB_EMPTY;
    doubles = BB_EMPTY;
    occupied[WHITE] = BB_EMPTY;
    occupied[BLACK] = BB_EMPTY;
    turn = WHITE;
}

void Board::setBoardState(const BoardState boardState) {
    singles = boardState.singles;
    doubles = boardState.doubles;
    occupied[WHITE] = boardState.occupied[WHITE];
    occupied[BLACK] = boardState.occupied[BLACK];
    turn = boardState.turn;
}

Color Board::getTurn() const {
    return turn;
}

Color Board::getResult() {
    if (isOver()) {
        return occupied[WHITE] ? BLACK : WHITE;
    }

    return NO_COLOR;
}

BoardState Board::getBoardState() {
    return {singles, doubles, occupied[WHITE], occupied[BLACK], turn};
}

Bitboard Board::getSingles(const Color color) const {
    return occupied[color] & singles;
}

Bitboard Board::getDoubles(const Color color) const {
    return occupied[color] & doubles;
}

void Board::setPieceAt(PieceType piece, Square square) {
    Bitboard bbSquare = BB_SQUARES[square];
    switch (piece) {
        case WHITE_SINGLE:
            singles |= bbSquare;
            occupied[WHITE] |= bbSquare;
            break;
        case WHITE_DOUBLE:
            doubles |= bbSquare;
            occupied[WHITE] |= bbSquare;
            break;
        case BLACK_SINGLE:
            singles |= bbSquare;
            occupied[BLACK] |= bbSquare;
            break;
        case BLACK_DOUBLE:
            doubles |= bbSquare;
            occupied[BLACK] |= bbSquare;
            break;
    }
}

int Board::getPieceCount(Color color) {
    return popCount(occupied[color]);
}
