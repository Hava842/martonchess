/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   File.h
 * Author: mh740
 *
 * Created on December 28, 2015, 9:21 AM
 */

#ifndef FILE_H
#define FILE_H

#include<array>

class File {
public:
    static const int a = 0;
    static const int b = 1;
    static const int c = 2;
    static const int d = 3;
    static const int e = 4;
    static const int f = 5;
    static const int g = 6;
    static const int h = 7;

    static const int NOFILE = 8;

    static const int VALUES_SIZE = 8;
    static const std::array<int, VALUES_SIZE> values;

    static bool isValid(int file);

private:
    File();
    ~File();
};

#endif /* FILE_H */

