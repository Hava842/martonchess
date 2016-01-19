
#include "MartonChess.h"

int main() {
    std::unique_ptr<MartonChess> martonchess(new MartonChess());
    martonchess->run();
    return 0;
}