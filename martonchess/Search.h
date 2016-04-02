
#ifndef SEARCH_H
#define SEARCH_H

#include "Evaluation.h"
#include "Protocol.h"
#include "Position.h"
#include "MoveGenerator.h"
#include "Timer.h"
#include "FenString.h"

#include <memory>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <atomic>

class Search {
public:
    Search(Protocol& protocol, double stageRatio, double cutoffRatio, bool enableBetaThreshold);

    void newSearch(Position& position, uint64_t searchTime);
    void reset();
    void resume();
    void suspend();
	void exit();
    void startTimer();

private:
    // Multi threading
    void run();
	int mainLoop(MoveList<RootEntry>& rootMoves, std::atomic<bool>& abortCondition);
    std::thread worker;
    std::atomic<bool> abort;
	std::atomic<bool> exitsearch;
    std::mutex wakeupmutex;
    std::condition_variable wakeupcondition;
    std::mutex suspendedmutex;
    std::condition_variable suspendedcondition;
    
    // Protocol
    Protocol& protocol;

    // Tools
    Evaluation evaluation;
    std::array<MoveGenerator, Depth::MAX_PLY> moveGenerators;
    
    // State
    Position& position = FenString::toPosition(FenString::STANDARDPOSITION);
    bool running;
    uint64_t searchTime;
    Timer timer;
	Timer stageTimer;
	std::atomic<bool> heavyStage;
    std::atomic<bool> timerAbort;


    // Search parameters
    MoveList<RootEntry> rootMoves;
    uint64_t totalNodes;
    int currentDepth;
    int currentMaxDepth;
    int currentMove;
    int currentMoveNumber;
    int bestResponse;
	double stageRatio;
	double cutoffRatio;
	bool enableBetaThreshold;

    void stopConditions();
    int search(int depth, int alpha, int beta, int ply, std::atomic<bool>& abortCondition);
};

#endif /* SEARCH_H */

