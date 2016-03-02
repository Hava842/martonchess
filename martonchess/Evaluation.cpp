
#include "Evaluation.h"
#include "PieceType.h"
#include "Value.h"

#include <cassert>

int Evaluation::materialWeight = 100;
int Evaluation::mobilityWeight = 60;
int Evaluation::centerWeight = 50;
int Evaluation::pawnStructureWeight = 50;
int Evaluation::kingSafetyWeight = 60;

int Evaluation::evaluatePawn(int color, Position& position) {
	assert(Color::isValid(color));
	int pawnScore = 0;

	int chainedPawnBonus = 10;
	int doublePawnPenalty = 20;
	int isolatedPawnPenalty = 20;
	int backwardPawnPenalty = 10;

	int chainedPawns = 0;
	int doublePawns = 0;
	int isolatedPawns = 0;
	int backwardPawns = 0;

	for (auto squares1 = position.pieces[color][PieceType::PAWN].squares; squares1 != 0; squares1 = Bitboard::remainder(squares1)) {
		int square1 = Bitboard::next(squares1);
		bool isolated = true;
		bool protected_ = false;
		bool stopcontrolled = false;

		for (auto squares2 = position.pieces[color][PieceType::PAWN].squares; squares2 != 0; squares2 = Bitboard::remainder(squares2)) {
			int square2 = Bitboard::next(squares2);
			if (square1 < square2) {
				// Chained pawns
				for (auto direction : Square::pawnDirections[color]) {
					int targetSquare = square1 + direction;
					chainedPawns += (targetSquare == square2) ? 1 : 0;
				}
				int targetWest = square1 + Square::W;
				if (Square::isValid(targetWest)) {
					chainedPawns += (targetWest == square2) ? 1 : 0;
				}
				int targetEast = square1 + Square::E;
				if (Square::isValid(targetEast)) {
					chainedPawns += (targetEast == square2) ? 1 : 0;
				}

				// Double pawns
				if (Square::getFile(square1) == Square::getFile(square2)) {
					doublePawns++;
				}

				
			}
			// Isolated pawns
			if (Square::getFile(square1) == Square::getFile(square2) + 1
				|| Square::getFile(square1) == Square::getFile(square2) - 1) {
				isolated = false;
			}

			// Protected by own pawn
			for (auto direction : Square::pawnDirections[color]) {
				int targetSquare = square2 + direction;
				if (targetSquare == square1) {
					protected_ = true;
				}
			}
		}

		// Stop square control
		int stopsquare1 = color == Color::WHITE ? square1 + Square::N : square1 + Square::S;
		for (auto enemysquares = position.pieces[Color::opposite(color)][PieceType::PAWN].squares; enemysquares != 0; enemysquares = Bitboard::remainder(enemysquares)) {
			int enemysquare = Bitboard::next(enemysquares);

			for (auto direction : Square::pawnDirections[Color::opposite(color)]) {
				int targetSquare = enemysquare + direction;
				if (targetSquare == stopsquare1) {
					stopcontrolled = true;
				}
			}
		}
		if (!protected_ && stopcontrolled) {
			backwardPawns++;
		}

	}

	return chainedPawns * chainedPawnBonus 
		- isolatedPawns * isolatedPawnPenalty 
		- backwardPawns * backwardPawnPenalty 
		- doublePawns * doublePawnPenalty;
}

int Evaluation::evaluateKingSafety(int color, Position& position) {
	assert(Color::isValid(color));

	auto pawns = position.pieces[color][PieceType::PAWN].squares;
	auto rooks = position.pieces[color][PieceType::ROOK].squares;
	int square = Bitboard::next(position.pieces[color][PieceType::KING].squares);

	Bitboard pawnShield;
	if (color == Color::WHITE) {
		if (Square::isValid(square + Square::NE)) { pawnShield.add(square + Square::NE); }
		if (Square::isValid(square + Square::N)) { pawnShield.add(square + Square::N); }
		if (Square::isValid(square + Square::NW)) { pawnShield.add(square + Square::NW); }
	}
	else {
		if (Square::isValid(square + Square::SE)) { pawnShield.add(square + Square::SE); }
		if (Square::isValid(square + Square::S)) { pawnShield.add(square + Square::S); }
		if (Square::isValid(square + Square::SW)) { pawnShield.add(square + Square::SW); }
	}
	int pawnShieldCount = Bitboard::bitCount(pawnShield.squares & pawns);

	Bitboard rookShield;
	if (Square::isValid(square + Square::E)) { pawnShield.add(square + Square::NE); }
	if (Square::isValid(square + Square::W)) { pawnShield.add(square + Square::W); }
	int rookShieldCount = Bitboard::bitCount(rookShield.squares & rooks);

	return pawnShieldCount * 10
		+ rookShieldCount * 50;
}

int Evaluation::evaluateCenter(int color, Position& position) {
	assert(Color::isValid(color));

	int pawnCenter = 0;
	for (auto squares = position.pieces[color][PieceType::PAWN].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		pawnCenter += evaluateCenter(color, position, square, Square::pawnDirections[color]);
	}

	int knightCenter = 0;
	for (auto squares = position.pieces[color][PieceType::KNIGHT].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		knightCenter += evaluateCenter(color, position, square, Square::knightDirections);
	}

	int bishopCenter = 0;
	for (auto squares = position.pieces[color][PieceType::BISHOP].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		bishopCenter += evaluateCenter(color, position, square, Square::bishopDirections);
	}

	int queenCenter = 0;
	for (auto squares = position.pieces[color][PieceType::QUEEN].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		queenCenter += evaluateCenter(color, position, square, Square::queenDirections);
	}

	return pawnCenter * 3
		+ knightCenter * 4
		+ bishopCenter * 4
		+ queenCenter;
}

int Evaluation::evaluateCenter(int color, Position& position, int square, const std::vector<int>& directions) {
	assert(Color::isValid(color));
	assert(Piece::isValid(position.board[square]));

	int center = Square::isCenter(square) ? 1 : 0;
	bool sliding = PieceType::isSliding(Piece::getType(position.board[square]));

	for (auto direction : directions) {
		int targetSquare = square + direction;

		while (Square::isValid(targetSquare)) {
			center += Square::isCenter(targetSquare) ? 1 : 0;

			if (sliding && position.board[targetSquare] == Piece::NOPIECE) {
				targetSquare += direction;
			}
			else {
				break;
			}
		}
	}

	return center;
}

int Evaluation::evaluate(Position& position, bool heavy) {
	// Initialize
	int myColor = position.activeColor;
	int oppositeColor = Color::opposite(myColor);
	int value = 0;

	// Evaluate material
	int materialScore = (evaluateMaterial(myColor, position) - evaluateMaterial(oppositeColor, position))
		* materialWeight / MAX_WEIGHT;
	value += materialScore;

	if (heavy) {
		// Evaluate mobility
		int mobilityScore = (evaluateMobility(myColor, position) - evaluateMobility(oppositeColor, position))
			* mobilityWeight / MAX_WEIGHT;
		value += mobilityScore;

		// Evaluate center control
		int centerScore = (evaluateCenter(myColor, position) - evaluateCenter(oppositeColor, position))
			* centerWeight / MAX_WEIGHT;
		value += centerScore;


		// Evaluate Pawn structure
		int pawnScore = (evaluatePawn(myColor, position) - evaluatePawn(oppositeColor, position))
			* pawnStructureWeight / MAX_WEIGHT;
		value += pawnScore;

		// Evaluate King Safety
		int kingScore = (evaluateKingSafety(myColor, position) - evaluateKingSafety(oppositeColor, position))
			* kingSafetyWeight / MAX_WEIGHT;
		value += kingSafetyWeight;
	}

	// Add Tempo
	value += TEMPO;

	assert(std::abs(value) < Value::CHECKMATE_THRESHOLD);
	return value;
}

int Evaluation::evaluateMaterial(int color, Position& position) {
	assert(Color::isValid(color));

	int material = position.material[color];

	// Add bonus for bishop pair
	if (position.pieces[color][PieceType::BISHOP].size() >= 2) {
		material += 50;
	}

	return material;
}

int Evaluation::evaluateMobility(int color, Position& position) {
	assert(Color::isValid(color));

	int knightMobility = 0;
	for (auto squares = position.pieces[color][PieceType::KNIGHT].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		knightMobility += evaluateMobility(color, position, square, Square::knightDirections);
	}

	int bishopMobility = 0;
	for (auto squares = position.pieces[color][PieceType::BISHOP].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		bishopMobility += evaluateMobility(color, position, square, Square::bishopDirections);
	}

	int rookMobility = 0;
	for (auto squares = position.pieces[color][PieceType::ROOK].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		rookMobility += evaluateMobility(color, position, square, Square::rookDirections);
	}

	int queenMobility = 0;
	for (auto squares = position.pieces[color][PieceType::QUEEN].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		queenMobility += evaluateMobility(color, position, square, Square::queenDirections);
	}

	return knightMobility * 4
		+ bishopMobility * 5
		+ rookMobility * 2
		+ queenMobility;
}

int Evaluation::evaluateMobility(int color, Position& position, int square, const std::vector<int>& directions) {
	assert(Color::isValid(color));
	assert(Piece::isValid(position.board[square]));

	int mobility = 0;
	bool sliding = PieceType::isSliding(Piece::getType(position.board[square]));

	for (auto direction : directions) {
		int targetSquare = square + direction;

		while (Square::isValid(targetSquare)) {
			++mobility;

			if (sliding && position.board[targetSquare] == Piece::NOPIECE) {
				targetSquare += direction;
			}
			else {
				break;
			}
		}
	}

	return mobility;
}

