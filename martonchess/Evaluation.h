
#ifndef EVALUATION_H
#define EVALUATION_H

#include "Position.h"

class Evaluation {
public:
    static const int TEMPO = 1;

    static int materialWeight;
    static int mobilityWeight;
	static int centerWeight;
	static int pawnStructureWeight;
	static int kingSafetyWeight;
	static int maxMaterial;

    int evaluate(Position& position, bool heavy, int beta, bool dumpoutput = false);

private:
    static const int MAX_WEIGHT = 100;
	static const int BETA_THRESHOLD = 150;
	
	double getMaterialRatio(int color, Position& position);
	int evaluateKingSafety(int color, Position& position);
	int evaluatePawn(int color, Position& position);
    int evaluateMaterial(int color, Position& position);
    int evaluateMobility(int color, Position& position);
    int evaluateMobility(int color, Position& position, int square, const std::vector<int>& directions);
	int evaluateCenter(int color, Position& position);
	int evaluateCenter(int color, Position& position, int square, const std::vector<int>& directions);
};


#endif /* EVALUATION_H */

