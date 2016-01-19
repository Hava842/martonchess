#ifndef VALUE_H
#define VALUE_H

#include "Depth.h"

class Value {
public:
    static const int INFINITE = 200000;
    static const int CHECKMATE = 100000;
    static const int CHECKMATE_THRESHOLD = CHECKMATE - Depth::MAX_PLY;
    static const int DRAW = 0;

    static const int NOVALUE = 300000;

    static bool isValid(int value);
    static bool isCheckmate(int value);

private:
    Value();
    ~Value();
};

#endif /* VALUE_H */

