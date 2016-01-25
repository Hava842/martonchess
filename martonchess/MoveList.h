
#ifndef MOVELIST_H
#define MOVELIST_H

#include "Value.h"
#include "Move.h"

#include <array>
#include <memory>

/**
 * This class stores our moves for a specific position. For the root node we
 * will populate pondermove for every root move.
 */
template<class T>
class MoveList {
private:
    static const int MAX_MOVES = 256;

public:
    std::array<std::shared_ptr<T>, MAX_MOVES> entries;
    int size = 0;

    MoveList();

    void sort();
    void rateFromMVVLVA();
};

class MoveVariation {
public:
    std::array<int, Depth::MAX_PLY> moves;
    int size = 0;
};

class MoveEntry {
public:
    int move = Move::NOMOVE;
    int value = Value::NOVALUE;
};

class RootEntry : public MoveEntry {
public:
    int pondermove = Move::NOMOVE;
};


#endif /* MOVELIST_H */

