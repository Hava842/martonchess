/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Rank.cpp
 * Author: mh740
 * 
 * Created on December 28, 2015, 9:22 AM
 */

#include "Rank.h"

const std::array<int, Rank::VALUES_SIZE> Rank::values = {
    r1, r2, r3, r4, r5, r6, r7, r8
};

bool Rank::isValid(int rank) {
    switch (rank) {
        case r1:
        case r2:
        case r3:
        case r4:
        case r5:
        case r6:
        case r7:
        case r8:
            return true;
        default:
            return false;
    }
}

