
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
  void ponderHit();

private:
  class Timer {
  public:
    Timer(std::atomic<bool>& timerRunning);

    void start(uint64_t searchTime);
    void kill();
  private:
    std::mutex mutex;
    std::condition_variable condition;
    std::thread thread;

    bool& timerRunning;
    void run(uint64_t searchTime);
  };
  
  void run();
  std::thread worker;
  std::atomic<bool> abort;
  std::mutex wakeupmutex;
  std::condition_variable wakeupcondition;
  std::mutex suspendedmutex;
  std::condition_variable suspendedcondition;
  Protocol& protocol;
  
  bool running;
  uint64_t searchTime;
  Timer timer;
  std::atomic<bool> timerRunning;
  bool timerMatters;

  Position position;
  Evaluation evaluation;

  // Move generator for each depth
  std::array<MoveGenerator, Depth::MAX_PLY> moveGenerators;

  // Search parameters
  MoveList<RootEntry> rootMoves;
  bool abort;
  uint64_t totalNodes;
  const int initialDepth = 1;
  int currentDepth;
  int currentMaxDepth;
  int currentMove;
  int currentMoveNumber;
  std::array<MoveVariation, Depth::MAX_PLY + 1> pv;

  void stopConditions();
  void searchRoot(int depth, int alpha, int beta);
  int search(int depth, int alpha, int beta, int ply);
  int quiescent(int depth, int alpha, int beta, int ply);
};

#endif /* SEARCH_H */

