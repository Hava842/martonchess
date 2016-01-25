#include "File.h"

const std::array<int, File::VALUES_SIZE> File::values = {
    a, b, c, d, e, f, g, h
};

bool File::isValid(int file) {
    switch (file) {
        case a:
        case b:
        case c:
        case d:
        case e:
        case f:
        case g:
        case h:
            return true;
        default:
            return false;
    }
}

