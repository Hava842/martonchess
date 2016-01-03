#ifndef CASTLINGTYPE_H
#define CASTLINGTYPE_H

#include <array>

class CastlingType {
public:
  static const int KINGSIDE = 0;
  static const int QUEENSIDE = 1;

  static const int NOCASTLINGTYPE = 2;

  static const int VALUES_SIZE = 2;
  static const std::array<int, VALUES_SIZE> values;

private:
  CastlingType();
  ~CastlingType();
};

#endif /* CASTLINGTYPE_H */

