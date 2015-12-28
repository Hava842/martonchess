/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CastlingType.h
 * Author: mh740
 *
 * Created on December 28, 2015, 9:36 AM
 */

#ifndef CASTLINGTYPE_H
#define CASTLINGTYPE_H

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

