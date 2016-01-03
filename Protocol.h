#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "MoveList.h"

class Protocol {
public:
  virtual ~Protocol() {};

  virtual void sendBestMove(int bestMove, int ponderMove) = 0;
  virtual void sendStatus(
    int currentDepth, int currentMaxDepth, uint64_t totalNodes, int currentMove, int currentMoveNumber) = 0;
  virtual void sendStatus(
    bool force, int currentDepth, int currentMaxDepth, uint64_t totalNodes, int currentMove, int currentMoveNumber) = 0;
  virtual void sendMove(RootEntry entry, int currentDepth, int currentMaxDepth, uint64_t totalNodes) = 0;
};
#endif /* PROTOCOL_H */

