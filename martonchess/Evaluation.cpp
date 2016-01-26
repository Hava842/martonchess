
#include "Evaluation.h"
#include "PieceType.h"
#include "Value.h"

#include <cassert>

int Evaluation::materialWeight = 100;
int Evaluation::mobilityWeight = 80;
int Evaluation::centerWeight = 60;

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

int Evaluation::evaluate(Position& position) {
	// Initialize
	int myColor = position.activeColor;
	int oppositeColor = Color::opposite(myColor);
	int value = 0;

	// Evaluate material
	int materialScore = (evaluateMaterial(myColor, position) - evaluateMaterial(oppositeColor, position))
		* materialWeight / MAX_WEIGHT;
	value += materialScore;

	// Evaluate mobility
	int mobilityScore = (evaluateMobility(myColor, position) - evaluateMobility(oppositeColor, position))
		* mobilityWeight / MAX_WEIGHT;
	value += mobilityScore;

	//Evaluate center control

	int centerScore = (evaluateCenter(myColor, position) - evaluateCenter(oppositeColor, position))
		* centerWeight / MAX_WEIGHT;
	value += centerScore;

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

/*
int Evaluation::evaluatePawn(int color, Position& position) {
assert(Color::isValid(color));
int halfOpenFiles = 0XFF;

// Loop through all pawns of the current color and score each pawn
for (auto squares = position.pieces[color][PieceType::PAWN].squares; squares != 0; squares = Bitboard::remainder(squares)) {
int square = Bitboard::next(squares);
int file = getFile(square);
int rank = getRank(square);

halfOpenFiles &= ~(1 << file);

previousLeftSquare = color == Color.WHITE ? square + Square.SW : square + Square.NW;
previousRightSquare = color == Color.WHITE ? square + Square.SE : square + Square.NE;

// Flag the pawn as passed, isolated, doubled or member of a pawn
// chain (but not the backward one).
bool chain = false;
if (Square.isValid(previousLeftSquare)) {
chain |= squares & Bitboard.toBitSquare(previousLeftSquare);
}
if (Square.isValid(previousRightSquare)) {
chain |= squares & Bitboard.toBitSquare(previousLeftSquare);
}

isolated = !squares & adjacent_files_bb(f));
doubled = squares & forward_bb(Us, s);
opposed = squares & forward_bb(Us, s);
passed = !(position.pieces[Color.opposite(color)][PieceType::PAWN].squares & passed_pawn_mask(color, square));

// Test for backward pawn
backward = false;

// If the pawn is passed, isolated, or member of a pawn chain it cannot
// be backward. If there are friendly pawns behind on adjacent files
// or if can capture an enemy pawn it cannot be backward either.
if (!(passed | isolated | chain)
&& !(ourPawns & attack_span_mask(Them, s))
&& !(pos.attacks_from<PAWN>(s, Us) & theirPawns)) {
// We now know that there are no friendly pawns beside or behind this
// pawn on adjacent files. We now check whether the pawn is
// backward by looking in the forward direction on the adjacent
// files, and seeing whether we meet a friendly or an enemy pawn first.
b = pos.attacks_from<PAWN>(s, Us);

// Note that we are sure to find something because pawn is not passed
// nor isolated, so loop is potentially infinite, but it isn't.
while (!(b & (ourPawns | theirPawns)))
Us == WHITE ? b <<= 8 : b >>= 8;

// The friendly pawn needs to be at least two ranks closer than the
// enemy pawn in order to help the potentially backward pawn advance.
backward = (b | (Us == WHITE ? b << 8 : b >> 8)) & theirPawns;
}

assert(opposed | passed | (attack_span_mask(Us, s) & theirPawns));

// A not passed pawn is a candidate to become passed if it is free to
// advance and if the number of friendly pawns beside or behind this
// pawn on adjacent files is higher or equal than the number of
// enemy pawns in the forward direction on the adjacent files.
candidate = !(opposed | passed | backward | isolated)
&& (b = attack_span_mask(Them, s + pawn_push(Us)) & ourPawns) != 0
&& popcount<Max15>(b) >= popcount<Max15>(attack_span_mask(Us, s) & theirPawns);

// Passed pawns will be properly scored in evaluation because we need
// full attack info to evaluate passed pawns. Only the frontmost passed
// pawn on each file is considered a true passed pawn.
if (passed && !doubled)
e->passedPawns[Us] |= s;

// Score this pawn
if (isolated)
value -= IsolatedPawnPenalty[opposed][f];

if (doubled)
value -= DoubledPawnPenalty[opposed][f];

if (backward)
value -= BackwardPawnPenalty[opposed][f];

if (chain)
value += ChainBonus[f];

if (candidate)
value += CandidateBonus[relative_rank(Us, s)];
}

return value;
}
*/
