//
// Created by Benedikt Hornig on 28.09.22.
//

#pragma once

#include <array>
#include <set>
#include "BaseAgent.h"
#include "TT.h"
#include "evaluate.h"

enum NodeType {
    ROOT_NODE,
    NON_ROOT_NODE
};

struct Node {
    int nodesSearched = 0;
    std::vector<Move> principalVariation = {};
};

struct MoveOrderEntry {
    MoveOrderEntry(Move m, int i);

    Move move;
    int value;
};

struct MoveOrderCompare {
    bool operator() (const MoveOrderEntry& lhs, const MoveOrderEntry& rhs) const {
        return lhs.value >= rhs.value;
    }
};

class AlphaBeta : public BaseAgent {
private:
    bool debug;
    std::uniform_int_distribution<int> uniRand;

    bool createdWholeTree = false;

    // move ordering
    std::set<MoveOrderEntry, MoveOrderCompare> moveOrderID;
    std::map<int, Move> killerMoves{};
    std::map<Move, int> historyHeuristic{};

    bool prevPVHit = false;
    std::vector<Move> prevPV{};
    BoardState prevBoard{BB_EMPTY, BB_EMPTY, BB_EMPTY, BB_EMPTY, WHITE};

    TT tt{};
    ZobristHash hashFunction{};

    // limit move time
    std::chrono::time_point<std::chrono::high_resolution_clock> moveStart;
    int moveTimeLimit = 0;

    template<NodeType nodeType>
    int search(Board state, Hash hashValue, Node &curNode, int depth, int alpha, int beta, int ply);

    evalConfig config{};
public:
    void setEvalConfig(const evalConfig &evalConfig);

public:
    explicit AlphaBeta(Color color, bool debug = true) : BaseAgent(color){
        this->debug = debug;
    }

    Move getMove(Board state) override;

    void reset(bool resetTT = false, bool resetHH = false);

    bool isTimeUp();

    void setMoveTimeLimit();

    void updateHH(Move move);

    int quiscenceSearch(Board state, int alpha, int beta);
};

