
#include "MoveType.h"

bool MoveType::isValid(int type) {
  switch (type) {
    case NORMAL:
    case PAWNDOUBLE:
    case PAWNPROMOTION:
    case ENPASSANT:
    case CASTLING:
      return true;
    default:
      return false;
  }
}

