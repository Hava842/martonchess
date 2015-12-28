/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Rank.h
 * Author: mh740
 *
 * Created on December 28, 2015, 9:22 AM
 */

#ifndef RANK_H
#define RANK_H

class Rank {
public:
  static const int r1 = 0;
  static const int r2 = 1;
  static const int r3 = 2;
  static const int r4 = 3;
  static const int r5 = 4;
  static const int r6 = 5;
  static const int r7 = 6;
  static const int r8 = 7;

  static const int NORANK = 8;

  static const int VALUES_SIZE = 8;
  static const std::array<int, VALUES_SIZE> values;

  static bool isValid(int rank);

private:
  Rank();
  ~Rank();
};


#endif /* RANK_H */

