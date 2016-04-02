#ifndef MARTONCHESS_H
#define MARTONCHESS_H

#include "Search.h"
#include "FenString.h"

class MartonChess : public Protocol {
public:
    void run();
	MartonChess(double stageRatio, double cutoffRatio, bool enableBetaThreshold);
    virtual void sendBestMove(int bestMove, int ponderMove);
    virtual void sendStatus(
            int currentDepth, int currentMaxDepth, uint64_t totalNodes, int currentMove, int currentMoveNumber);
    virtual void sendStatus(
            bool force, int currentDepth, int currentMaxDepth, uint64_t totalNodes, int currentMove, int currentMoveNumber);
    virtual void sendMove(RootEntry entry, int currentDepth, int currentMaxDepth, uint64_t totalNodes);

    static std::string fromMove(int move);

private:
    std::unique_ptr<Search> search;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point statusStartTime;

    std::unique_ptr<Position> currentPosition = std::unique_ptr<Position>(new Position(FenString::toPosition(FenString::STANDARDPOSITION)));

    void receiveInitialize();
    void receiveReady();
    void receiveNewGame();
    void receivePosition(std::istringstream& input);
    void receiveGo(std::istringstream& input);
    void receivePonderHit();
    void receiveStop();
    void receiveQuit();
};

#endif /* MARTONCHESS_H */

