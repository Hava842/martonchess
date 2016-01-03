
#include "Value.h"

#include <cassert>
#include <cmath>

bool Value::isValid(int value) {
  int absvalue = std::abs(value);

  return absvalue <= CHECKMATE || absvalue == INFINITE;
}

bool Value::isCheckmate(int value) {
  assert(isValid(value));

  int absvalue = std::abs(value);

  return absvalue >= CHECKMATE_THRESHOLD && absvalue <= CHECKMATE;
}
