
#ifndef EVALUATION_H
#define EVALUATION_H

#include "Position.h"

class Evaluation {
public:
    static const int TEMPO = 1;

    static int materialWeight;
    static int mobilityWeight;
	static int centerWeight;

    int evaluate(Position& position);

private:
    static const int MAX_WEIGHT = 100;

    int evaluateMaterial(int color, Position& position);
    int evaluateMobility(int color, Position& position);
    int evaluateMobility(int color, Position& position, int square, const std::vector<int>& directions);
	int evaluateCenter(int color, Position& position);
	int evaluateCenter(int color, Position& position, int square, const std::vector<int>& directions);
};


#endif /* EVALUATION_H */

