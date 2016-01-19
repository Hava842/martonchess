#include "Search.h"

#include <iostream>
#include <string>
#include <cassert>

Search::Timer::Timer(std::atomic<bool>& timerRunning)
    : timerRunning(timerRunning) {
}

void Search::Timer::run(uint64_t searchTime) {
  std::unique_lock<std::mutex> lock(mutex);
  if (condition.wait_for(lock, std::chrono::milliseconds(searchTime)) == std::cv_status::timeout) {
    timerRunning = true;
  }
}

void Search::Timer::start(uint64_t searchTime) {
  if (thread.joinable()) {
    thread.join();
  }
  thread = std::thread(&Search::Timer::run, this, searchTime);
}

void Search::Timer::kill() {
  condition.notify_all();
  thread.join();
}

Search::Search(Protocol& protocol)
    : protocol(protocol),
    timer(timerRunning){

  reset();

  worker = std::thread(&Search::run, this);
}

void Search::reset() {
  std::assert(!running);
  searchTime = 0;
  timer.kill();
  running = false;
  totalNodes = 0;
  currentDepth = initialDepth;
  currentMaxDepth = 0;
  currentMove = Move::NOMOVE;
  currentMoveNumber = 0;
}

void Search::resume() {
  wakeupcondition.notify_all();
}

void Search::suspend() {
  abort = true;
  std::unique_lock<std::mutex> lock(suspendedmutex);
  suspendedcondition.wait(lock);
}

void Search::run() {
  while (true) {
    std::unique_lock<std::mutex> lock(wakeupmutex);
    wakeupcondition.wait(lock);
    
    for {
        //stuff
        if (abort) {
            break;
        }
    }
    
    running = false;
    suspendedcondition.notify_all();
  }
}

void Search::stopConditions() {
  if (!timerRunning && timerMatters) {
    abort = true;
  } else {
    if (rootMoves.size == 1) {
      abort = true;
    } else
      if (Value::isCheckmate(rootMoves.entries[0]->value)) {
        abort = true;
      }
  }
}
