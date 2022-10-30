//
// Created by Benedikt Hornig on 29.09.22.
//

#include "TT.h"
#include <stdexcept>

const u_int64_t LAST_32_BITS = uint32_t(~0); // all ones for last 32 bits
const u_int64_t FIRST_32_BITS = LAST_32_BITS << 32;

TTEntry::TTEntry(Hash hash, const Move move, const int value, const ValueFlag flag, const int depth) {
    hashKey = hash;
    bestMove = move;
    this->value = value;
    valueFlag = flag;
    searchDepth = depth;
}

TTEntry::TTEntry() {
    hashKey = 0;
    bestMove = NONE_MOVE;
    value = 0;
    valueFlag = EXACT;
    searchDepth = -1;
}

TTEntry TT::lookupEntry(const Hash hashValue) {
    HashKey primHash = (hashValue & FIRST_32_BITS) >> 32;
    if (lookupTable.contains(primHash)) {
        HashKey secHash = hashValue & LAST_32_BITS;

        if (lookupTable.at(primHash)[0].hashKey == secHash)
            return lookupTable.at(primHash)[0];
        else if (lookupTable.at(primHash)[1].hashKey == secHash)
            return lookupTable.at(primHash)[1];
    }

    return {0, NONE_MOVE, 0, EXACT, -1};
}

void TT::store(const Board &board, const Hash hashValue, const Move move, const int value, const ValueFlag flag, const int depth) {
    HashKey hashIdx = (hashValue & FIRST_32_BITS) >> 32;
    HashKey hashKey = hashValue & LAST_32_BITS;

    if (lookupTable.contains(hashIdx)) {
        // Two Big comparison
        if (lookupTable.at(hashIdx)[1].searchDepth <= depth) {
            lookupTable.at(hashIdx)[0] = TTEntry(hashKey, move, value, flag, depth);
        } else {
            lookupTable.at(hashIdx)[1] = TTEntry(hashKey, move, value, flag, depth);
        }
    } else {
        std::array<TTEntry, 2> entry;
        entry[0] = TTEntry(hashKey, move, value, flag, depth);
        lookupTable.emplace(hashIdx, entry);
    }
}

void TT::clearTable() {
    lookupTable.clear();
}
