#include "Search.h"

#include <iostream>
#include <string>
#include <cassert>

Search::Search(Protocol& protocol, double stageRatio, double cutoffRatio)
	: protocol(protocol),
	stageRatio(stageRatio),
	cutoffRatio(cutoffRatio),
	timer([&]()
{
	timerAbort = true;
}),
stageTimer([&]()
{
	heavyStage = true;
}),
exitsearch(false) {
	reset();
	worker = std::thread(&Search::run, this);
}

void Search::newSearch(Position& position, uint64_t searchTime) {
	reset();

	this->position = position;
	this->searchTime = searchTime;
	timer.setInterval(std::chrono::milliseconds(searchTime));
	stageTimer.setInterval(std::chrono::milliseconds((int) (searchTime * stageRatio)));

}

void Search::reset() {
	searchTime = 0;
	timer.stop();
	stageTimer.stop();
	timerAbort = false;
	heavyStage = false;
	running = false;
	abort = false;
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

void Search::exit() {
	exitsearch = true;
	suspend();
	resume();
	if (worker.joinable()) {
		worker.join();
	}
	timer.stop();
	stageTimer.stop();
}

void Search::startTimer() {
	timer.start(true);
	stageTimer.start(true);
}

int Search::mainLoop( MoveList<RootEntry>& rootMoves, std::atomic<bool>& abortCondition) {
	int targetCutoff = rootMoves.size;
	for (int depth = 0; depth <= Depth::MAX_DEPTH; depth++) {

		// reset rootMove values
		for (int i = 0; i < rootMoves.size; i++) {
			rootMoves.entries[i]->value = -Value::INFINITE;
		}

		int alpha = -Value::INFINITE;
		int beta = Value::INFINITE;

		for (int i = 0; i < rootMoves.size; i++) {
			int move = rootMoves.entries[i]->move;
			position.makeMove(move);
			int value = -search(depth, -beta, -alpha, 1, abortCondition);
			//protocol.sendBestMove(move, Move::NOMOVE);
			position.undoMove(move);

			// If aborted we must not update the value.
			stopConditions();
			if (abort || abortCondition) {
				break;
			}

			rootMoves.entries[i]->value = value;
			rootMoves.entries[i]->pondermove = bestResponse;

			if (value > alpha) {
				alpha = value;
			}
		}
		if (abort || abortCondition) {
			break;
		}

		rootMoves.sort();
		targetCutoff = (int)(rootMoves.size * cutoffRatio);
		while (targetCutoff < rootMoves.size) {
			if (rootMoves.entries[targetCutoff - 1]->value == rootMoves.entries[targetCutoff]->value) {
				targetCutoff++;
			}
			else {
				break;
			}
		}

	}

	return targetCutoff;
}

void Search::run() {
	while (!exitsearch) {
		std::unique_lock<std::mutex> lock(wakeupmutex);
		wakeupcondition.wait(lock);
		if (exitsearch) {
			break;
		}
		running = true;

		//Populate rootMoves
		MoveList<MoveEntry>& rootMovesRef = moveGenerators[0].getLegalMoves(position, 1, position.isCheck());
		rootMoves.size = rootMovesRef.size;
		int targetCutoff = rootMoves.size;
		for (int i = 0; i < rootMovesRef.size; i++) {
			rootMoves.entries[i]->move = rootMovesRef.entries[i]->move;
		}

		// Stage 1
		if (stageRatio > 0) {
			targetCutoff = mainLoop(rootMoves, heavyStage);
		}
		else {
			heavyStage = true;
		}

		// Cutoff
		rootMoves.size = targetCutoff;

		// Stage 2
		if (heavyStage && !abort) {
			mainLoop(rootMoves, abort);
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

int Search::search(int depth, int alpha, int beta, int ply, std::atomic<bool>& abortCondition) {
	if (ply >= Depth::MAX_PLY) {
		return evaluation.evaluate(position, heavyStage);
	}

	if (position.isRepetition() || position.hasInsufficientMaterial() || position.halfmoveClock >= 100) {
		return Value::DRAW;
	}

	int bestValue = -Value::INFINITE;

	if (!position.isCheck() && depth <= 0) {
		bestValue = evaluation.evaluate(position, heavyStage);
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
			value = -search(depth - 1, -beta, -alpha, ply + 1, abortCondition);
		}
		else {
			value = bestValue;
		}
		position.undoMove(move);

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

		stopConditions();
		if (abortCondition) {
			return bestValue;
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
	}
	else {
		if (rootMoves.size == 1) {
			abort = true;
		}
	}
}
