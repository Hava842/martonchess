/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Color.h
 * Author: mh740
 *
 * Created on December 28, 2015, 9:33 AM
 */

#ifndef COLOR_H
#define COLOR_H

#include <array>

class Color {
public:
  static const int WHITE = 0;
  static const int BLACK = 1;

  static const int NOCOLOR = 2;

  static const int VALUES_SIZE = 2;
  static const std::array<int, VALUES_SIZE> values;

  static bool isValid(int color);
  static int opposite(int color);

private:
  Color();
  ~Color();
};

#endif /* COLOR_H */

