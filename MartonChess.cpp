#include "MartonChess.h"
#include "CastlingType.h"
#include "File.h"
#include "Rank.h"

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

void MartonChess::run() {
    std::cin.exceptions(std::iostream::eofbit | std::iostream::failbit | std::iostream::badbit);
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        std::istringstream input(line);

        std::string token;
        input >> std::skipws >> token;
        if (token == "uci") {
            receiveInitialize();
        } else if (token == "isready") {
            receiveReady();
        } else if (token == "ucinewgame") {
            receiveNewGame();
        } else if (token == "position") {
            receivePosition(input);
        } else if (token == "go") {
            receiveGo(input);
        } else if (token == "stop") {
            receiveStop();
        } else if (token == "ponderhit") {
            receivePonderHit();
        } else if (token == "quit") {
            receiveQuit();
            break;
        }
    }
}

void MartonChess::receiveQuit() {
    // We received a quit command. Stop calculating now and
    // cleanup!
    search->suspend();
}

void MartonChess::receiveInitialize() {
    search->suspend();

    // We received an initialization request.

    // We could do some global initialization here. Probably it would be best
    // to initialize all tables here as they will exist until the end of the
    // program.

    // We must send an initialization answer back!
    std::cout << "id name MartonChess" << std::endl;
    std::cout << "id author MHavasi" << std::endl;
    std::cout << "uciok" << std::endl;
}

void MartonChess::receiveReady() {
    // We received a ready request. We must send the token back as soon as we
    // can. However, because we launch the search in a separate thread, our main
    // thread is able to handle the commands asynchronously to the search. If we
    // don't answer the ready request in time, our engine will probably be
    // killed by the GUI.
    std::cout << "readyok" << std::endl;
}

void MartonChess::receiveNewGame() {
    search->suspend();

    // We received a new game command.

    // Initialize per-game settings here.
    *currentPosition = FenString::toPosition(FenString::STANDARDPOSITION);
}

void MartonChess::receivePosition(std::istringstream& input) {
    search->suspend();

    // We received an position command. Just setup the position.

    std::string token;
    input >> token;
    if (token == "startpos") {
        *currentPosition = FenString::toPosition(FenString::STANDARDPOSITION);

        if (input >> token) {
            if (token != "moves") {
                throw std::exception();
            }
        }
    } else if (token == "fen") {
        std::string fen;

        while (input >> token) {
            if (token == "moves") {
                break;
            } else {
                fen += token + " ";
            }
        }

        *currentPosition = FenString::toPosition(fen);
    } else {
        throw std::exception();
    }

    MoveGenerator moveGenerator;

    while (input >> token) {
        // Verify moves
        MoveList<MoveEntry>& moves = moveGenerator.getLegalMoves(*currentPosition, 1, currentPosition->isCheck());
        bool found = false;
        for (int i = 0; i < moves.size; ++i) {
            int move = moves.entries[i]->move;
            if (fromMove(move) == token) {
                currentPosition->makeMove(move);
                found = true;
                break;
            }
        }

        if (!found) {
            throw std::exception();
        }
    }

    // Don't start searching though!
}

void MartonChess::receiveGo(std::istringstream& input) {
    search->suspend();

    // We received a start command. Extract all parameters from the
    // command and start the search.
    std::string token;
    input >> token;
    if (token == "movetime") {
        uint64_t searchTime;
        if (input >> searchTime) {
            search->newSearch(*currentPosition, searchTime);
        }
    } else {
        uint64_t whiteTimeLeft = 1;
        uint64_t whiteTimeIncrement = 0;
        uint64_t blackTimeLeft = 1;
        uint64_t blackTimeIncrement = 0;
        int searchMovesToGo = 40;
        bool ponder = false;

        do {
            if (token == "wtime") {
                if (!(input >> whiteTimeLeft)) {
                    throw std::exception();
                }
            } else if (token == "winc") {
                if (!(input >> whiteTimeIncrement)) {
                    throw std::exception();
                }
            } else if (token == "btime") {
                if (!(input >> blackTimeLeft)) {
                    throw std::exception();
                }
            } else if (token == "binc") {
                if (!(input >> blackTimeIncrement)) {
                    throw std::exception();
                }
            } else if (token == "movestogo") {
                if (!(input >> searchMovesToGo)) {
                    throw std::exception();
                }
            } else if (token == "ponder") {
                ponder = true;
            }
        } while (input >> token);

        uint64_t searchTime;
        if (currentPosition->activeColor == Color::WHITE) {
            searchTime = (whiteTimeLeft / searchMovesToGo) - 1000;
        } else {
            searchTime = (blackTimeLeft / searchMovesToGo) - 1000;
        }
        
        if (ponder) {
            search->newSearch(*currentPosition, searchTime);
            search->startTimer();
        } else {
            search->newSearch(*currentPosition, searchTime);
        }
    }

    search->resume();
    startTime = std::chrono::system_clock::now();
    statusStartTime = startTime;
}

void MartonChess::receivePonderHit() {
    search->startTimer();
    
}

void MartonChess::receiveStop() {
    // We received a stop command. If a search is running, stop it.
    search->suspend();
}

void MartonChess::sendBestMove(int bestMove, int ponderMove) {
    std::cout << "bestmove ";

    if (bestMove != Move::NOMOVE) {
        std::cout << fromMove(bestMove);

        if (ponderMove != Move::NOMOVE) {
            std::cout << " ponder " << fromMove(ponderMove);
        }
    } else {
        std::cout << "nomove";
    }

    std::cout << std::endl;
}

void MartonChess::sendStatus(
        int currentDepth, int currentMaxDepth, uint64_t totalNodes, int currentMove, int currentMoveNumber) {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - statusStartTime).count() >= 1000) {
        sendStatus(false, currentDepth, currentMaxDepth, totalNodes, currentMove, currentMoveNumber);
    }
}

void MartonChess::sendStatus(
        bool force, int currentDepth, int currentMaxDepth, uint64_t totalNodes, int currentMove, int currentMoveNumber) {
    auto timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime);

    if (force || timeDelta.count() >= 1000) {
        std::cout << "info";
        std::cout << " depth " << currentDepth;
        std::cout << " seldepth " << currentMaxDepth;
        std::cout << " nodes " << totalNodes;
        std::cout << " time " << timeDelta.count();
        std::cout << " nps " << (timeDelta.count() >= 1000 ? (totalNodes * 1000) / timeDelta.count() : 0);

        if (currentMove != Move::NOMOVE) {
            std::cout << " currmove " << fromMove(currentMove);
            std::cout << " currmovenumber " << currentMoveNumber;
        }

        std::cout << std::endl;

        statusStartTime = std::chrono::system_clock::now();
    }
}

void MartonChess::sendMove(RootEntry entry, int currentDepth, int currentMaxDepth, uint64_t totalNodes) {
    auto timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime);

    std::cout << "info";
    std::cout << " depth " << currentDepth;
    std::cout << " seldepth " << currentMaxDepth;
    std::cout << " nodes " << totalNodes;
    std::cout << " time " << timeDelta.count();
    std::cout << " nps " << (timeDelta.count() >= 1000 ? (totalNodes * 1000) / timeDelta.count() : 0);

    if (std::abs(entry.value) >= Value::CHECKMATE_THRESHOLD) {
        // Calculate mate distance
        int mateDepth = Value::CHECKMATE - std::abs(entry.value);
        std::cout << " score mate " << ((entry.value > 0) - (entry.value < 0)) * (mateDepth + 1) / 2;
    } else {
        std::cout << " score cp " << entry.value;
    }

    /*if (entry.pv.size > 0) {
        std::cout << " pv";
        for (int i = 0; i < entry.pv.size; ++i) {
            std::cout << " " << fromMove(entry.pv.moves[i]);
        }
    }*/

    std::cout << std::endl;

    statusStartTime = std::chrono::system_clock::now();
}

std::string MartonChess::fromMove(int move) {
    std::string notation;

    notation += FenString::fromSquare(Move::getOriginSquare(move));
    notation += FenString::fromSquare(Move::getTargetSquare(move));

    int promotion = Move::getPromotion(move);
    if (promotion != PieceType::NOPIECETYPE) {
        notation += std::tolower(FenString::fromPieceType(promotion));
    }

    return notation;
}