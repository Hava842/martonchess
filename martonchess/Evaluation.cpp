
#include "Evaluation.h"
#include "PieceType.h"
#include "Value.h"

#include <cassert>
#include <iostream>

int Evaluation::materialWeight = 100;
int Evaluation::mobilityWeight = 100;
int Evaluation::centerWeight = 100;
int Evaluation::pawnStructureWeight = 100;
int Evaluation::kingSafetyWeight = 100;
int Evaluation::maxMaterial = 8*100 + 4*325 + 2*500 + 975 + 20000 + 50;

int Evaluation::evaluatePawn(int color, Position& position) {
	assert(Color::isValid(color));
	int pawnScore = 0;

	int chainedPawnBonus = 20;
	int doublePawnPenalty = 20;
	int isolatedPawnPenalty = 20;
	int backwardPawnPenalty = 20;
	int doubleisolatedPawnPenalty = 70;

	double chainedPawns = 0;
	int doublePawns = 0;
	int isolatedPawns = 0;
	int backwardPawns = 0;
	int doubleisolatedPawns = 0;

	for (auto squares1 = position.pieces[color][PieceType::PAWN].squares; squares1 != 0; squares1 = Bitboard::remainder(squares1)) {
		int square1 = Bitboard::next(squares1);
		bool isolated = true;
		bool double_ = false;
		bool protected_ = false;
		bool stopcontrolled = false;

		for (auto squares2 = position.pieces[color][PieceType::PAWN].squares; squares2 != 0; squares2 = Bitboard::remainder(squares2)) {
			int square2 = Bitboard::next(squares2);
			if (square1 == square2) {
				continue;
			}
			// Chained pawns
			for (auto direction : Square::pawnDirections[color]) {
				if (direction == Square::N || direction == Square::S)
					continue;
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
				double_ = true;
			}
			// Isolated pawns
			if (Square::getFile(square1) == Square::getFile(square2) + 1
				|| Square::getFile(square1) == Square::getFile(square2) - 1) {
				isolated = false;
			}

			// Protected by own pawn
			for (auto direction : Square::pawnDirections[color]) {
				int targetSquare = square2 + direction;
				if (direction == Square::N || direction == Square::S)
					continue;
				if (targetSquare == square1) {
					protected_ = true;
				}
			}
		}

		if (isolated) {
			isolatedPawns++;
		}

		if (double_) {
			doublePawns++;
		}

		if (isolated && double_) {
			doubleisolatedPawns++;
		}

		// Stop square control
		int stopsquare1 = color == Color::WHITE ? square1 + Square::N : square1 + Square::S;
		for (auto enemysquares = position.pieces[Color::opposite(color)][PieceType::PAWN].squares; enemysquares != 0; enemysquares = Bitboard::remainder(enemysquares)) {
			int enemysquare = Bitboard::next(enemysquares);

			if (enemysquare == stopsquare1) {
				stopcontrolled = true;
			}
			for (auto direction : Square::pawnDirections[Color::opposite(color)]) {
				if (direction == Square::N || direction == Square::S)
					continue;
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
		- (doublePawns / 2) * doublePawnPenalty
		- (doubleisolatedPawns / 2) * doubleisolatedPawnPenalty;
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

	int rookShieldCount = 0;
	for (auto rookSquares = position.pieces[color][PieceType::ROOK].squares; rookSquares != 0; rookSquares = Bitboard::remainder(rookSquares)) {
		int rookSquare = Bitboard::next(rookSquares);
		for (auto rookDirection : Square::rookDirections) {
			int targetSquare = rookSquare + rookDirection;

			int cover = 0;
			while (Square::isValid(targetSquare)) {
				for (auto kingDirection : Square::kingDirections) {
					if (targetSquare == square + kingDirection)
						rookShieldCount++;
				}

				if (position.board[targetSquare] != Piece::NOPIECE) {
					cover++;
				}

				if (cover < 2) {
					targetSquare += rookDirection;
				}
				else {
					break;
				}
			}
		}
	}
	

	return pawnShieldCount * 20
		+ rookShieldCount * 10;
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
		+ bishopCenter * 2
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

double Evaluation::getMaterialRatio(int color, Position& position) {
	int myColor = position.activeColor;
	int mymaterial = evaluateMaterial(myColor, position);
	return mymaterial / maxMaterial;
}

int Evaluation::evaluate(Position& position, bool heavy, int beta, bool dumpoutput) {
	// Initialize
	int myColor = position.activeColor;
	int oppositeColor = Color::opposite(myColor);
	int value = 0;

	// Evaluate material
	int mymaterial = evaluateMaterial(myColor, position);
	int materialScore = (mymaterial - evaluateMaterial(oppositeColor, position))
		* materialWeight / MAX_WEIGHT;



	value += materialScore;

	if (dumpoutput) {
		std::cout << " materialscore: " << mymaterial * materialWeight / MAX_WEIGHT << " " << materialScore;
	}

	if (value >= beta + BETA_THRESHOLD) {
		return value;
	}

	if (heavy) {
		double materialRatio =(double) mymaterial / maxMaterial;

		// Evaluate mobility
		int mymobility = evaluateMobility(myColor, position);
		int mobilityScore = (mymobility - evaluateMobility(oppositeColor, position))
			* mobilityWeight / MAX_WEIGHT;
		value += mobilityScore;
		if (dumpoutput) {
			std::cout << " mobilityscore: " << mymobility * mobilityWeight / MAX_WEIGHT << " " << mobilityScore;
		}

		// Evaluate center control
		int mycentre = evaluateCenter(myColor, position);
		int centerScore = (mycentre - evaluateCenter(oppositeColor, position))
			* materialRatio * centerWeight / MAX_WEIGHT;
		value += centerScore;
		if (dumpoutput) {
			std::cout << " centrescore: " << int (mycentre * materialRatio * centerWeight / MAX_WEIGHT) << " " << centerScore;
		}

		// Evaluate Pawn structure
		int mypawn = evaluatePawn(myColor, position);
		int pawnScore = (mypawn - evaluatePawn(oppositeColor, position))
			* pawnStructureWeight / MAX_WEIGHT;
		value += pawnScore;
		if (dumpoutput) {
			std::cout << " pawnscore: " << int(mypawn * pawnStructureWeight / MAX_WEIGHT) << " " << pawnScore;
		}

		// Evaluate King Safety
		int myking = evaluateKingSafety(myColor, position);
		int kingSafetyScore = (myking - evaluateKingSafety(oppositeColor, position))
			* materialRatio * kingSafetyWeight / MAX_WEIGHT;
		value += kingSafetyScore;
		if (dumpoutput) {
			std::cout << " kingscore: " << int(myking * materialRatio * kingSafetyWeight / MAX_WEIGHT) << " " << kingSafetyScore;
		}
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

	int kingMobility = 0;
	for (auto squares = position.pieces[color][PieceType::KING].squares; squares != 0; squares = Bitboard::remainder(squares)) {
		int square = Bitboard::next(squares);
		kingMobility += evaluateMobility(color, position, square, Square::kingDirections);
	}

	return knightMobility * 3
		+ bishopMobility * 4
		+ rookMobility 
		+ queenMobility
		- kingMobility ;
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

