/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Board.cpp
 * Author: mh740
 * 
 * Created on December 28, 2015, 8:07 AM
 */

#include "Position.h"

Position::Position(std::string fenString) {
    
    const std::string FenString::STANDARDPOSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    whitePawns = 0;
    whiteKnights = 0;
    whiteBishops = 0;
    whiteRooks = 0;
    whiteQueens = 0;
    whiteKing = 0;

    blackPawns = 0;
    blackKnights = 0;
    blackBishops = 0;
    blackRooks = 0;
    blackQueens = 0;
    blackKing = 0;
    
    std::vector<std::string> fenVector;
    boost::split(strs, sample, boost::is_any_of("/"));
    for (int i = 0; i < 8; i++) {
        int xpos = 0;
        for (char& c : fenVector[i]) {
            switch (c) {
                case ''
            }
        }
    }
    activeColor = 0;
    
    whiteCastleKingside = 1;
    whiteCastleQueenside = 1;
    blackCastleKingside = 1;
    blackCastleQueenside = 1;
    
    enpassant = "-";
    
    halfmoveClock = 0;
    
    fullmoveClock = 1;
}

Board::Board(const Board& orig) {
}

Board::~Board() {
}

