
#include "MartonChess.h"

#include <stdlib.h>     /* atof */

int main(int argc, char* argv[]) {
	double stageRatio = 0;
	double cutoffRatio = 1;
	if (argc > 2) {
		stageRatio = atof(argv[1]);
		cutoffRatio = atof(argv[2]);
	}

	if (stageRatio > 1 || stageRatio < 0) {
		stageRatio = 0;
	}

	if (cutoffRatio > 1 || cutoffRatio < 0) {
		cutoffRatio = 1;
	}
    std::unique_ptr<MartonChess> martonchess(new MartonChess(stageRatio, cutoffRatio));
    martonchess->run();
    return 0;
}