//
// Created by Benedikt Hornig on 28.09.22.
//

#include <iostream>
#include "AlphaBeta.h"

const int MAX_INT = 1000;

MoveOrderEntry::MoveOrderEntry(Move m, int i) {
    move = m;
    value = i;
}

void updatePrincipalVariation(std::vector<Move> &pv, Move move, const std::vector<Move> &childPv, const int prevChildPvSize) {
    // erase previous childPv
    if (prevChildPvSize > 0)
        pv.erase(pv.end() - prevChildPvSize, pv.end());

    // erase previous primary move
    pv.erase(pv.end() - 1, pv.end());

    // add new move to pv and append childPv after
    pv.push_back(move);
    for (const auto &childMove: childPv)
        pv.push_back(childMove);
}

void AlphaBeta::updateHH(Move move) {
    // updates the history heuristic
    if (historyHeuristic.contains(move)) {
        historyHeuristic[move]++;
    } else {
        historyHeuristic.emplace(move, 1);
    }
}

template<NodeType nodeType>
int AlphaBeta::search(Board state, Hash hashValue, Node& curNode, const int depth, int alpha, int beta, int ply) {
    if (isTimeUp()) {
        return MAX_INT;
    }

    // transposition table lookup
    int oldAlpha = alpha;
    TTEntry entry = tt.lookupEntry(hashValue);
    if (entry.searchDepth >= depth) {
        switch (entry.valueFlag) {
            case EXACT:
                updateHH(entry.bestMove);
                curNode.principalVariation.push_back(entry.bestMove);
                return entry.value;
            case UPPER_BOUND:
                alpha = std::min(alpha, entry.value);
                break;
            case LOWER_BOUND:
                beta = std::min(beta, entry.value);
                break;
        }

        if (alpha >= beta) {
            updateHH(entry.bestMove);
            curNode.principalVariation.push_back(entry.bestMove);
            curNode.nodesSearched++;
            return entry.value;
        }
    }

    std::vector<Move> moves = state.getLegalMoves();

    // return value or perform quiscence search if crowning moves are possible
    if (state.isOver() || depth == 0) {
        if (!state.isOver()) {
            // if the search reached a non-terminal node the whole game tree was not searched
            createdWholeTree = false;

            // quiscence search if crowning moves are possible
            for (const auto &move: moves) {
                if (move.isCrowning()) {
                    return quiscenceSearch(state, -beta, -alpha);
                }
            }
        }

        curNode.nodesSearched++;
        return evaluate(state, config);
    }

    assert(!moves.empty());

    // move ordering
    int moveOrderIdx = 0;

    // check last principal variation at root node
    if (nodeType == ROOT_NODE && prevPV.size() > 2) {
        Board pBoard = Board();
        pBoard.setBoardState(prevBoard);
        pBoard.makeMove(prevPV[0]);
        pBoard.makeMove(prevPV[1]);

        if (pBoard.getBoardState() == state.getBoardState()) {
            // update killer moves if it is a hit
            prevPVHit = true;
            for (int i = 3; i < prevPV.size(); ++i) {
                killerMoves[i] = prevPV[i];
            }
        }
    }

    // search previous PV moves first, if there was a hit
    if (prevPVHit && ply <= 3) {
        auto loc = std::find(moves.begin(), moves.end(), prevPV[ply]);
        if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
            std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
            moveOrderIdx++;
        }
    }

    // push best move from tt entry forward
    if (entry.searchDepth != -1 && entry.searchDepth < depth) {
        auto loc = std::find(moves.begin(), moves.end(), entry.bestMove);
        if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
            std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
            moveOrderIdx++;
        }
    }

    // push killer moves forward
    if (killerMoves.contains(depth)) {
        auto loc = std::find(moves.begin(), moves.end(), killerMoves[depth]);
        if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
            std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
            moveOrderIdx++;
        }
    }

    // push crowning and bear off moves forward
    for (int i = moveOrderIdx; i < moves.size(); ++i) {
        if (moves[i].isCrowning() || BB_SQUARES[moves[i].getToSquare()] & (state.getTurn() == WHITE ? BB_RANK_1 : BB_RANK_8)) {
            auto loc = std::find(moves.begin(), moves.end(), moves[i]);
            if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
                std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
                moveOrderIdx++;
            }
        }
    }

    // push longer moves forward
    std::vector<std::pair<Move, int>> longMoves;
    for (int i = moveOrderIdx; i < moves.size(); ++i) {
        longMoves.emplace_back(moves[i], std::abs(squareRank(moves[i].getToSquare()) - squareRank(moves[i].getFromSquare())));
    }
    if (!longMoves.empty()) {
        std::sort(longMoves.begin(), longMoves.end(), [](std::pair<Move, int> p1, std::pair<Move, int> p2){return p1.second < p2.second;});
        for (const auto &move: longMoves) {
            auto loc = std::find(moves.begin(), moves.end(), move.first);
            if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
                std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
                moveOrderIdx++;
            }
        }
    }

    // push history heuristic moves forward
    std::vector<std::pair<Move,int>> hhMoves;
    for (int i = moveOrderIdx; i < moves.size(); ++i) {
        if (historyHeuristic.contains(moves[i])) {
            // collect legal moves with a history heuristic table entry
            hhMoves.emplace_back(moves[i], historyHeuristic[moves[i]]);
        }
    }

    if (!hhMoves.empty()) {
        // sort the moves by their occurrence
        std::sort(hhMoves.begin(), hhMoves.end(), [](std::pair<Move, int> p1, std::pair<Move, int> p2) {return p1.second < p2.second;});
        for (const auto &hhMove: hhMoves) {
            // push moves forward
            auto loc = std::find(moves.begin(), moves.end(), hhMove.first);
            if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
                std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
                moveOrderIdx++;
            }
        }
    }

    // order moves from previous root node of last ID iteration at the root node
    if (nodeType == ROOT_NODE && !moveOrderID.empty()) {
        for (const auto &moveOrderEntry: moveOrderID) {
            auto loc = std::find(moves.begin(), moves.end(), moveOrderEntry.move);
            if (loc != moves.end() && loc > moves.begin() + moveOrderIdx) {
                std::rotate(moves.begin() + moveOrderIdx, loc, loc + 1);
                moveOrderIdx++;
            }
        }

        moveOrderID.clear();
    }

    Node childNode;
    int prevChildPvSize = 0;
    int score;
    curNode.principalVariation.push_back(NONE_MOVE);

    // principal variation search
    Move pvMove = moves.front();
    Hash newHash = hashFunction.continuousHash(hashValue, state, pvMove);
    state.makeMove(pvMove);

    score = -search<NON_ROOT_NODE>(state, newHash, childNode, depth - 1, -beta, -alpha, ply + 1);
    state.undoMove();
    moves.erase(moves.begin());

    curNode.nodesSearched += childNode.nodesSearched;
    updatePrincipalVariation(curNode.principalVariation, pvMove, childNode.principalVariation, prevChildPvSize);
    prevChildPvSize = (int) childNode.principalVariation.size();

    if (nodeType == ROOT_NODE) {
        // root node move ordering for next ID iteration
        moveOrderID.emplace(MoveOrderEntry(pvMove, score));
    }

    if (score < beta) {
        for (auto &move : moves) {
            childNode = Node();

            int lowerBound = std::max(score, alpha);
            int upperBound = lowerBound + 1;

            newHash = hashFunction.continuousHash(hashValue, state, move);
            state.makeMove(move);
            int result = -search<NON_ROOT_NODE>(state, newHash, childNode, depth - 1, -upperBound, -lowerBound, ply+1);
            curNode.nodesSearched += childNode.nodesSearched;

            if (result >= upperBound && result < beta) {
                // re-search
                childNode = Node();
                result = -search<NON_ROOT_NODE>(state, newHash, childNode, depth - 1, -beta, -result, ply + 1);
                curNode.nodesSearched += childNode.nodesSearched;
            }

            state.undoMove();

            if (result > score) {
                updatePrincipalVariation(curNode.principalVariation, move, childNode.principalVariation, prevChildPvSize);
                prevChildPvSize = (int) childNode.principalVariation.size();
                score = result;
            }

            if (nodeType == ROOT_NODE) {
                // root node move ordering from last ID iteration
                moveOrderID.emplace(MoveOrderEntry(move, result));
            }

            if (result >= beta) {
                killerMoves[depth] = move;
                break;
            }
        }
    }

    if (!isTimeUp()) {
        // update transposition table and history heuristic
        ValueFlag flag = EXACT;
        if (score <= oldAlpha) {
            flag = UPPER_BOUND;
        } else if (score >= beta) {
            flag = LOWER_BOUND;
        }

        tt.store(state, hashValue, curNode.principalVariation.front(), score, flag, depth);
        updateHH(curNode.principalVariation.front());
    }

    curNode.nodesSearched++;
    assert(curNode.principalVariation.front() != NONE_MOVE || isTimeUp());
    return score;
}

int AlphaBeta::quiscenceSearch(Board state, int alpha, int beta) {
    // quiscence search on crowning moves
    // https://www.chessprogramming.org/Quiescence_Search
    int standPat = evaluate(state, config);
    if(standPat >= beta )
        return beta;
    if(alpha < standPat )
        alpha = standPat;

    for( auto &move : state.getLegalMoves() )  {
        if (!move.isCrowning())
            // only investigate crowning moves
            continue;
        state.makeMove(move);
        int score = -quiscenceSearch(state, -beta, -alpha);
        state.undoMove();

        if( score >= beta )
            return beta;
        if( score > alpha )
            alpha = score;
    }

    return alpha;
}

void AlphaBeta::setMoveTimeLimit() {
    moveTimeLimit = 9;
}

bool AlphaBeta::isTimeUp() {
    // check if there is no time left to move
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - moveStart).count() >= moveTimeLimit;
}

Move AlphaBeta::getMove(Board state) {
    reset(true);

    if (state.getLegalMoves().size() == 1) {
        // return early if there is only one move
        prevPV.clear();
        prevBoard = state.getBoardState();

        return state.getLegalMoves().front();
    }

    setMoveTimeLimit();
    moveStart = std::chrono::high_resolution_clock::now();
    moveOrderID.clear();

    Node root, lastRoot;
    Hash hashValue = hashFunction.initialHash(state);

    // time run duration for last search depth
    using namespace std::chrono;
    high_resolution_clock::time_point depthStartTime = high_resolution_clock::now();
    auto timeForLastDepth = milliseconds::zero();

    int depth = 0, nodesSearched = 0, eval = 0;
    while (!isTimeUp() && !createdWholeTree && eval != 1000
            // don't search another iteration if the last iteration took longer than the remaining move time
            && duration_cast<seconds>(timeForLastDepth).count() < moveTimeLimit - duration_cast<seconds>(high_resolution_clock::now() - depthStartTime).count()) {
        depthStartTime = high_resolution_clock::now();

        createdWholeTree = true;
        depth++;
        root = Node();
        eval = search<ROOT_NODE>(state, hashValue, root, depth, -MAX_INT, MAX_INT, 0);

        if (!isTimeUp()) {
            // save last search result
            assert(!root.principalVariation.empty() && root.principalVariation.front() != NONE_MOVE);
            lastRoot = root;
            nodesSearched += lastRoot.nodesSearched;
            timeForLastDepth = duration_cast<milliseconds>(high_resolution_clock::now() - depthStartTime);

            if (debug) {
                std::cout << "Depth " << depth << ": " << timeForLastDepth.count() << "ms, ";
                std::cout << "Nodes searched: " << nodesSearched << ", ";
                std::cout << "Eval: " << eval << ", ";
                std::cout << "Best move: " << std::string(lastRoot.principalVariation.front()) << std::endl;
            }
        }
    }

    prevPV = lastRoot.principalVariation;
    prevBoard = state.getBoardState();

    return lastRoot.principalVariation.front();
}

void AlphaBeta::reset(bool resetTT, bool resetHH) {
    // reset all parameters of the last search that are not needed/wanted
    moveOrderID.clear();
    prevPVHit = false;
    createdWholeTree = false;
    killerMoves.clear();
    if (resetHH)
        historyHeuristic.clear();
    if (resetTT)
        tt.clearTable();
}

void AlphaBeta::setEvalConfig(const evalConfig &evalConfig) {
    AlphaBeta::config = evalConfig;
}
