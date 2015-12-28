/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   File.cpp
 * Author: mh740
 * 
 * Created on December 28, 2015, 9:21 AM
 */

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

