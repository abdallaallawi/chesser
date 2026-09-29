#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>


class Piece {
	int square;			//The number of the square that the piece is occupying
	char name;			//Piece name ie pawn, knight, etc.
	bool white;			// Color; white = true, black = false

	bool moved = false;		//Has the piece moved at least once?

public:
	Piece(int square, char name, bool white);
	void setSquare(int square);
	int getSquare();
	char getName();
	bool getColor();
	bool getMoved();
	Piece* promote(Piece* piece, char to);
};

class Move {
	int from;
	int to;
public:
	Move();
	bool pawn = false;
	bool capture = false;
	void setFrom(int from);
	void setTo(int to);
	int getFrom();
	int getTo();
	static std::vector<Move> moves;
};

class Board {


	bool whiteTurn = true;					//Is it white's turn?

	bool check = false;
	bool checkmate = false;
	bool draw = false;


	std::vector<std::string> positions;

	void recordPos();

	int checker = -1;  //The last piece to give a check

	int enPassant = -1; //The square that triggers enPassant if moved to

	int castle = -1; //The square that triggers castling if moved to

	std::pair<char, bool> searchPiece(int square);   //Search if a piece is occupying a certain square, returns the name and the color, or '0' if not


	bool isCheck(int square, bool color, bool king);  //To detect a check on a king, or to check if there's a certain square that is attacked by 
	//opposition piece so that the king is blocked from going to that square, color refers to player's color
	//white = true, black = false || king boolean indicates if there's an actual check or not, if true,
	//the square in which the piece responsible for checking will be stored in "checker" variable


	bool isCheckmate(Piece& King);   //Check if the state of the game is checkmate

	bool isDraw(bool color);			//Check if the state of the game is draw
	//Color only matters here to detect stalemate, other drawing conditions don't require color


public:
	int getEnPassant();
	int getCastle();
	std::vector<Piece> pieces;
	std::vector<Piece> captured;		//Captured pieces
	Board();					//Classic chess game
	Board(std::string position, bool whoseTurn);					//Set up pieces and positions 

	static Board createWithParams(std::string fenString, bool isWhite) {
        return Board(fenString, isWhite);
    }

	std::string getLastPosition();
	bool getCheckmate();
	bool getCheck();
	bool getDraw();

	bool getTurn();			//White = true, black = false

	Piece* selectPiece(char name, int pos);		//First select a piece before moving it, don't move a piece without selecting it first

	bool pushMove(int to, Piece* piece);   //Apply a move

	std::vector<int> availableMoves(Piece& piece, int pos);  //Generate every legal move for a certain piece

	std::vector<int> availableMovesCheck(Piece& piece, int pos); //Generate every legal move for a certain piece during a check

};