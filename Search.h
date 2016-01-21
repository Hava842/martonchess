
#ifndef SEARCH_H
#define SEARCH_H

#include "Evaluation.h"
#include "Protocol.h"
#include "Position.h"
#include "MoveGenerator.h"

#include <memory>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <atomic>

class Search {
public:
    Search(Protocol& protocol);

    void newSearch(Position& position, uint64_t searchTime);
    void reset();
    void resume();
    void suspend();
    void startTimer();

private:

    class Timer {
    public:
        Timer(std::atomic<bool>& timerAbort);

        void start(uint64_t searchTime);
        void kill();
    private:
        std::mutex mutex;
        std::condition_variable condition;
        std::thread thread;

        std::atomic<bool>& timerAbort;
        void run(uint64_t searchTime);
    };

    // Multi threading
    void run();
    std::thread worker;
    std::atomic<bool> abort;
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
    Position position;
    bool running;
    uint64_t searchTime;
    Timer timer;
    std::atomic<bool> timerAbort;


    // Search parameters
    MoveList<RootEntry> rootMoves;
    uint64_t totalNodes;
    int currentDepth;
    int currentMaxDepth;
    int currentMove;
    int currentMoveNumber;
    int bestResponse;

    void stopConditions();
    int search(int depth, int alpha, int beta, int ply);
};

#endif /* SEARCH_H */

