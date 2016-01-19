#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "Position.h"
#include "MoveList.h"

class MoveGenerator {
public:
    MoveList<MoveEntry>& getLegalMoves(Position& position, int depth, bool isCheck);
    MoveList<MoveEntry>& getMoves(Position& position, int depth, bool isCheck);

private:
    MoveList<MoveEntry> moves;

    void addMoves(MoveList<MoveEntry>& list, Position& position);
    void addMoves(MoveList<MoveEntry>& list, int originSquare, const std::vector<int>& directions, Position& position);
    void addPawnMoves(MoveList<MoveEntry>& list, int pawnSquare, Position& position);
    void addCastlingMoves(MoveList<MoveEntry>& list, int kingSquare, Position& position);
};

#endif /* MOVEGENERATOR_H */

