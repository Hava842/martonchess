#include "Search.h"

#include <iostream>
#include <string>
#include <cassert>

Search::Timer::Timer(std::atomic<bool>& timerAbort)
: timerAbort(timerAbort) {
}

void Search::Timer::run(uint64_t searchTime) {
    std::unique_lock<std::mutex> lock(mutex);
    if (condition.wait_for(lock, std::chrono::milliseconds(searchTime)) == std::cv_status::timeout) {
        timerAbort = true;
    }
}

void Search::Timer::start(uint64_t searchTime) {
    
    thread = std::thread(&Search::Timer::run, this, searchTime);
}

void Search::Timer::kill() {
    condition.notify_all();
    if (thread.joinable()) {
        thread.join();
    }
}

Search::Search(Protocol& protocol): protocol(protocol), timer(timerAbort) {

    reset();

    worker = std::thread(&Search::run, this);
}

void Search::newSearch(Position& position, uint64_t searchTime) {
    reset();
    
    this->position = position;
    this->searchTime = searchTime;
    
}

void Search::reset() {
    searchTime = 0;
    timer.kill();
    timerAbort = false;
    running = false;
    bestResponse = Move::NOMOVE;
}

void Search::resume() {
    wakeupcondition.notify_all();
}

void Search::suspend() {
    std::unique_lock<std::mutex> lock(suspendedmutex);
    if (running) {
        abort = true;
        suspendedcondition.wait(lock);
    }
    
}

void Search::startTimer() {
    timer.start(searchTime);
}

void Search::run() {
    while (true) {
        std::unique_lock<std::mutex> lock(wakeupmutex);
        wakeupcondition.wait(lock);
        running = true;

        //Populate rootMoves
        MoveList<MoveEntry>& rootMovesRef = moveGenerators[0].getLegalMoves(position, 1, position.isCheck());
        rootMoves.size = rootMovesRef.size;
        for (int i = 0; i < rootMovesRef.size; i++) {
            rootMoves.entries[i]->move = rootMovesRef.entries[i]->move;
        }
        
        for (int depth = 2; depth <= 2 /*Depth::MAX_DEPTH*/; depth++) {
            // reset rootMove values
            for (int i = 0; i < rootMoves.size; i++) {
                rootMoves.entries[i]->value = -Value::INFINITE;
            }

            int alpha = -Value::INFINITE;
            int beta = Value::INFINITE;

            for (int i = 0; i < rootMoves.size; i++) {
                int move = rootMoves.entries[i]->move;
                position.makeMove(move);
                int value = -search(depth, -beta, -alpha, 1);
                rootMoves.entries[i]->value = value;
                std::cout << " rootmove value: " << value;
                protocol.sendBestMove(move, Move::NOMOVE);
                position.undoMove(move);

                if (value > alpha) {
                    alpha = value;
                    rootMoves.entries[i]->pondermove = bestResponse;
                }
            }

            rootMoves.sort();

            stopConditions();

            if (abort) {
                break;
            }
        }

        int bestMove = 0;
        int ponderMove = 0;
        if (rootMoves.size > 0) {
            bestMove = rootMoves.entries[0]->move;
            ponderMove = rootMoves.entries[0]->pondermove;
        }
        protocol.sendBestMove(bestMove, ponderMove);

        running = false;
        suspendedcondition.notify_all();
    }
}

int Search::search(int depth, int alpha, int beta, int ply) {
    if (abort || depth >= Depth::MAX_PLY) {
        return evaluation.evaluate(position);
    }

    if (position.isRepetition() || position.hasInsufficientMaterial() || position.halfmoveClock >= 100) {
        return Value::DRAW;
    }

    int bestValue = -Value::INFINITE;

    if (!position.isCheck() && depth <= 0) {
        bestValue = evaluation.evaluate(position);
        if (bestValue > alpha) {
            alpha = bestValue;
            if (bestValue >= beta) {
                return bestValue;
            }
        }
    }

    auto possibleMoves = moveGenerators[ply].getMoves(position, depth, position.isCheck());
    bool haveValidMove = false;
    for (int i = 0; i < possibleMoves.size; i++) {
        int move = possibleMoves.entries[i]->move;
        int value = -Value::INFINITE;
        
        position.makeMove(move);
        if (!position.isCheck(Color::opposite(position.activeColor))) {
            haveValidMove = true;
            value = -search(depth - 1, -beta, -alpha, ply + 1);
        }
        position.undoMove(move);

        stopConditions();
        
        if (abort) {
            return bestValue;
        }
        
        if (value > bestValue) {
            bestValue = value;
        }
        
        if (value > alpha) {
            alpha = value;
            bestResponse = move;
            if (value >= beta) {
                break;
            }
        }
    }
    
    //Checkmate
    if (!haveValidMove && position.isCheck()) {
        return -Value::CHECKMATE;
    }
    
    //Stalemate
    if (!haveValidMove && depth > 0) {
        return Value::DRAW;
    }
    
    return bestValue;
}

void Search::stopConditions() {
    if (timerAbort) {
        abort = true;
    } else {
        if (rootMoves.size == 1) {
            abort = true;
        }
    }
}
