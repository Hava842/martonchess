#include "Color.h"

const std::array<int, Color::VALUES_SIZE> Color::values = {
    WHITE, BLACK
};

bool Color::isValid(int color) {
    switch (color) {
        case WHITE:
        case BLACK:
            return true;
        default:
            return false;
    }
}

int Color::opposite(int color) {
    switch (color) {
        case WHITE:
            return BLACK;
        case BLACK:
            return WHITE;
        default:
            throw std::exception();
    }
}

