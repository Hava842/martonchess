/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Marci
 *
 * Created on November 6, 2015, 4:05 PM
 */

#include <iostream>
#include <string>

using namespace std;

int main ()
{
	string Line; //to read the command given by the GUI
	int flag =1; //to change the value of chess squares from 'a' to 'h'

	cout.setf (ios::unitbuf);// Make sure that the outputs are sent straight away to the GUI

	while( getline( cin, Line ) ) {
		if ( Line == "uci" ) {
			cout << "id name martonchess" << endl;
			cout << "id author XXX" << endl;
			cout << "uciok" << endl;
		} else if ( Line == "quit" ) {
			cout << "Bye Bye" << endl;
			break;
		} else if ( Line == "isready" ) {
			cout << "readyok" << endl;
		} else if ( Line == "ucinewgame" ) {
			; // nothing to do
		}

		if ( Line.substr(0,23) == "position startpos moves " ) {
			; // nothing to do
		} else if ( Line == "stop" ) {
			; // nothing to do
		} else if ( Line.substr( 0, 3 ) == "go " ) {
			// Received a command like: "go wtime 300000 btime 300000 winc 0 binc 0"
			cout << "bestmove " << char(105-flag) << "7" << char(105-flag) << "5" << endl;
			//Output like: "bestmove h7h5"
			flag++; //increase flag to move other pawn on next turn
		}
	}

	return 0;
}
