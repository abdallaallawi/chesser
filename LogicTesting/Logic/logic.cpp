#include "logic.h"


Piece::Piece(int square, char name, bool white) {
	this->square = square;
	this->name = name;
	this->white = white;
}

void Piece::setSquare(int square) {
	this->square = square;
	moved = true;
}

int Piece::getSquare() {
	return square;
}

char Piece::getName() {
	return name;
}

bool Piece::getColor() {
	return white;
}

bool Piece::getMoved() {
	return moved;
}

Piece* Piece::promote(Piece* piece, char to) {
	if (name != 'P') return NULL;
	if (piece->getColor()) {
		if (piece->getSquare() / 8 == 7) {
			piece->name = to;
			return piece;
		}
	}
	else {
		if (piece->getSquare() / 8 == 0) {
			piece->name = to;
			return piece;
		}
	}
}

Move::Move() {
	to = -1;
	from = -1;
}

std::vector<Move> Move::moves;

void Move::setFrom(int from) {
	this->from = from;
}
void Move::setTo(int to) {
	this->to = to;
}

int Move::getFrom() {
	return from;
}

int Move::getTo() {
	return to;
}

Board::Board() {
	for (int i = 8; i < 16; i++) {
		pieces.push_back(Piece(i, 'P', true));
		pieces.push_back(Piece(i + 40, 'P', false));
	}
	pieces.push_back(Piece(0, 'R', true));
	pieces.push_back(Piece(7, 'R', true));
	pieces.push_back(Piece(1, 'N', true));
	pieces.push_back(Piece(6, 'N', true));
	pieces.push_back(Piece(2, 'B', true));
	pieces.push_back(Piece(5, 'B', true));
	pieces.push_back(Piece(3, 'Q', true));
	pieces.push_back(Piece(4, 'K', true));

	pieces.push_back(Piece(63, 'R', false));
	pieces.push_back(Piece(56, 'R', false));
	pieces.push_back(Piece(62, 'N', false));
	pieces.push_back(Piece(57, 'N', false));
	pieces.push_back(Piece(61, 'B', false));
	pieces.push_back(Piece(58, 'B', false));
	pieces.push_back(Piece(59, 'Q', false));
	pieces.push_back(Piece(60, 'K', false));

	positions.push_back("RNBQKBNRPPPPPPPP00000000000000000000000000000000pppppppprnbqkbnr");	//captial letter = white, small leter = black,
	// 0 = empty square
}

Board::Board(std::string position, bool whoseTurn) {
	whiteTurn = whoseTurn;

	for (int i = 0; i < position.size(); i++) {
		if (position[i] == '0') continue;
		bool white = false;
		char piece = position[i];
		int ascii = piece;
		if (ascii < 97) white = true;
		if (!white) {
			ascii -= 32;
			piece = ascii;
		}
		pieces.push_back(Piece(i, piece, white));
	}
	recordPos();
}

std::string Board::getLastPosition() {
	return positions[positions.size() - 1];
}

bool Board::getCheckmate() {
	return checkmate;
}
bool Board::getCheck() {
	return check;
}
bool Board::getDraw() {
	return draw;
}

bool Board::getTurn() {
	return whiteTurn;
}

int Board::getEnPassant() {
	return enPassant;
}

int Board::getCastle() {
	return castle;
}

std::pair<char, bool> Board::searchPiece(int square) {
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getSquare() == square) {
			return { pieces[i].getName(), pieces[i].getColor() };
		}
	}
	return { '0', 0 };
}

Piece* Board::selectPiece(char name, int pos) {
	Piece* temp = NULL;
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getName() == name && pieces[i].getSquare() == pos) {
			temp = &pieces[i];
		}
	}
	if (temp != NULL && temp->getColor() != whiteTurn) return NULL;
	return temp;
}


bool Board::isCheck(int square, bool color, bool king) {
	int row = square / 8;
	double col = square / 8.0;
	col = round((col - row) * 10 / 1.25);		//This block to find out the number of squares between the parameter and the edge of the board
	row = 7 - row;								//row = number of squares until the right edge of the board
	col = 7 - col;								//col = number of squares until the top edge of the board

	//Queen, Rook and Bishop
	std::pair<char, bool> temp;
	for (int i = 1; i <= row; i++) {
		temp = searchPiece(square + 8 * i);
		if ((temp.first == 'Q' || temp.first == 'R')) {
			if (temp.second != color) {
				if (king) checker = square + 8 * i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	for (int i = 1; i <= static_cast<int>(col); i++) {
		temp = searchPiece(square + i);
		if ((temp.first == 'Q' || temp.first == 'R')) {
			if (temp.second != color) {
				if (king) checker = square + i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	for (int i = 1; i <= (7 - row); i++) {
		temp = searchPiece(square - 8 * i);
		if ((temp.first == 'Q' || temp.first == 'R')) {
			if (temp.second != color) {
				if (king) checker = square - 8 * i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	for (int i = 1; i <= static_cast<int>(7 - col); i++) {
		temp = searchPiece(square - i);
		if ((temp.first == 'Q' || temp.first == 'R')) {
			if (temp.second != color) {
				if (king) checker = square - i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	int topRight = std::min(row, static_cast<int>(col));
	int botRight = std::min((7 - row), static_cast<int>(col));
	int botLeft = std::min((7 - row), static_cast<int>(7 - col));
	int topLeft = std::min(row, static_cast<int>(7 - col));

	for (int i = 1; i <= topRight; i++) {
		temp = searchPiece(square + 9 * i);
		if ((temp.first == 'Q' || temp.first == 'B')) {
			if (temp.second != color) {
				if (king) checker = square + 9 * i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	for (int i = 1; i <= botRight; i++) {
		temp = searchPiece(square - 7 * i);
		if ((temp.first == 'Q' || temp.first == 'B')) {
			if (temp.second != color) {
				if (king) checker = square - 7 * i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	for (int i = 1; i <= botLeft; i++) {
		temp = searchPiece(square - 9 * i);
		if ((temp.first == 'Q' || temp.first == 'B')) {
			if (temp.second != color) {
				if (king) checker = square - 9 * i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	for (int i = 1; i <= topLeft; i++) {
		temp = searchPiece(square + 7 * i);
		if ((temp.first == 'Q' || temp.first == 'B')) {
			if (temp.second != color) {
				if (king) checker = square + 7 * i;
				return true;
			}
			if (temp.second == color) break;
		}
		else if (temp.first != '0') break;
	}
	//Knight
	if (row > 1 && col > 0) {
		if (searchPiece(square + 17).first == 'N' && searchPiece(square + 17).second != color) {
			if (king) checker = square + 17;
			return true;
		}
	}
	if (row > 1 && col < 7) {
		if (searchPiece(square + 15).first == 'N' && searchPiece(square + 15).second != color) {
			if (king) checker = square + 15;
			return true;
		}
	}
	if (col > 1 && row > 0) {
		if (searchPiece(square + 10).first == 'N' && searchPiece(square + 10).second != color) {
			if (king) checker = square + 10;
			return true;
		}
	}
	if (col > 1 && row < 7) {
		if (searchPiece(square - 6).first == 'N' && searchPiece(square - 6).second != color) {
			if (king) checker = square - 6;
			return true;
		}
	}
	if (row < 6 && col > 0) {
		if (searchPiece(square - 15).first == 'N' && searchPiece(square - 15).second != color) {
			if (king) checker = square - 15;
			return true;
		}
	}
	if (row < 6 && col < 7) {
		if (searchPiece(square - 17).first == 'N' && searchPiece(square - 17).second != color) {
			if (king) checker = square - 17;
			return true;
		}
	}
	if (col < 6 && row < 7) {
		if (searchPiece(square - 10).first == 'N' && searchPiece(square - 10).second != color) {
			if (king) checker = square - 10;
			return true;
		}
	}
	if (col < 6 && row > 0) {
		if (searchPiece(square + 6).first == 'N' && searchPiece(square + 6).second != color) {
			if (king) checker = square + 6;
			return true;
		}
	}

	//Pawn and King
	if (row < 7) {
		if (searchPiece(square - 8).first == 'K' && searchPiece(square - 8).second != color) return true;
		if (col < 7) {
			if (searchPiece(square - 9).first == 'K' && searchPiece(square - 9).second != color) return true;
		}
		if (col > 0) {
			if (searchPiece(square - 7).first == 'K' && searchPiece(square - 7).second != color) return true;
		}
	}
	if (col < 7) {
		if (searchPiece(square - 1).first == 'K' && searchPiece(square - 1).second != color) return true;
	}
	if (col > 0) {
		if (searchPiece(square + 1).first == 'K' && searchPiece(square + 1).second != color) return true;
	}
	if (row > 0) {
		if (searchPiece(square + 8).first == 'K' && searchPiece(square + 8).second != color) return true;
		if (col > 0) {
			if (searchPiece(square + 9).first == 'K' && searchPiece(square + 9).second != color) return true;
		}
		if (col < 7) {
			if (searchPiece(square + 7).first == 'K' && searchPiece(square + 7).second != color) return true;
		}
	}

	if (color && searchPiece(square + 9).first == 'P' && !searchPiece(square + 9).second && col > 0 && row > 0) {
		if (king) checker = square + 9;
		return true;
	}
	if (color && searchPiece(square + 7).first == 'P' && !searchPiece(square + 7).second && col < 7 && row > 0) {
		if (king) checker = square + 7;
		return true;
	}

	if (!color && searchPiece(square - 7).first == 'P' && searchPiece(square - 7).second && col > 0 && row < 7) {
		if (king) checker = square - 7;
		return true;
	}
	if (!color && searchPiece(square - 9).first == 'P' && searchPiece(square - 9).second && col < 7 && row < 7) {
		if (king) checker = square - 9;
		return true;
	}
	return false;
}


std::vector<int> Board::availableMoves(Piece& piece, int pos) {
	int row = -1;
	double col = -1;

	//To find out the number of squares between the piece itself and the edge of the board |||
	//row = number of rows between the piece itself and the edge of the board
	//col = number of columns between the piece itself and the edge of the board
	row = pos / 8;
	col = pos / 8.0;
	col = round((col - row) * 10 / 1.25);

	row = 7 - row;
	col = 7 - col;

	std::vector<int> moves;

	//King movement rules
	if (piece.getName() == 'K') {

		int square = piece.getSquare();
		bool color = piece.getColor();

		if (row < 7) {
			if (searchPiece(square - 8).first != '0' && searchPiece(square - 8).second != color) {
				if (!isCheck(square - 8, color, false)) moves.push_back(square - 8);
			}
			if (searchPiece(square - 8).first == '0') {
				if (!isCheck(square - 8, color, false)) moves.push_back(square - 8);
			}


			if (col < 7) {
				if (searchPiece(square - 9).first != '0' && searchPiece(square - 9).second != color) {
					if (!isCheck(square - 9, color, false)) moves.push_back(square - 9);
				}
				if (searchPiece(square - 9).first == '0') {
					if (!isCheck(square - 9, color, false)) moves.push_back(square - 9);
				}
			}
			if (col > 0) {
				if (searchPiece(square - 7).first != '0' && searchPiece(square - 7).second != color) {
					if (!isCheck(square - 7, color, false)) moves.push_back(square - 7);
				}
				if (searchPiece(square - 7).first == '0') {
					if (!isCheck(square - 7, color, false)) moves.push_back(square - 7);
				}
			}
		}
		if (col < 7) {

			if (searchPiece(square - 1).first != '0' && searchPiece(square - 1).second != color) {
				if (!isCheck(square - 1, color, false)) moves.push_back(square - 1);
			}
			if (searchPiece(square - 1).first == '0') {
				if (!isCheck(square - 1, color, false)) moves.push_back(square - 1);
			}
		}
		if (col > 0) {

			if (searchPiece(square + 1).first != '0' && searchPiece(square + 1).second != color) {
				if (!isCheck(square + 1, color, false)) moves.push_back(square + 1);
			}
			if (searchPiece(square + 1).first == '0') {
				if (!isCheck(square + 1, color, false)) moves.push_back(square + 1);
			}
		}
		if (row > 0) {

			if (searchPiece(square + 8).first != '0' && searchPiece(square + 8).second != color) {
				if (!isCheck(square + 8, color, false)) moves.push_back(square + 8);
			}
			if (searchPiece(square + 8).first == '0') {
				if (!isCheck(square + 8, color, false)) moves.push_back(square + 8);
			}

			if (col < 7) {
				if (searchPiece(square + 7).first != '0' && searchPiece(square + 7).second != color) {
					if (!isCheck(square + 7, color, false)) moves.push_back(square + 7);
				}
				if (searchPiece(square + 7).first == '0') {
					if (!isCheck(square + 7, color, false)) moves.push_back(square + 7);
				}
			}
			if (col > 0) {
				if (searchPiece(square + 9).first != '0' && searchPiece(square + 9).second != color) {
					if (!isCheck(square + 9, color, false)) moves.push_back(square + 9);
				}
				if (searchPiece(square + 9).first == '0') {
					if (!isCheck(square + 9, color, false)) moves.push_back(square + 9);
				}
			}
		}
		// Castling Kingside and Queenside
		if (isCheck(piece.getSquare(), piece.getColor(), true)) return moves;

		if (!piece.getMoved()) {

			std::string temp1 = "";
			std::string temp2 = "";
			if (piece.getColor()) {
				for (int i = 4; i < 8; i++) {
					temp1 += positions[positions.size() - 1][i];
				}
				if (temp1 == "K00R") {
					if (!selectPiece('R', 7)->getMoved()) {
						if (!isCheck(4, piece.getColor(), false) && !isCheck(5, piece.getColor(), false)
							&& !isCheck(6, piece.getColor(), false) && !isCheck(7, piece.getColor(), false)) {
							moves.push_back(6);
							castle = 6;
						}
					}
				}
				for (int i = 0; i < 5; i++) {
					temp2 += positions[positions.size() - 1][i];
				}
				if (temp2 == "R000K") {
					if (!selectPiece('R', 0)->getMoved()) {
						if (!isCheck(0, piece.getColor(), false) && !isCheck(1, piece.getColor(), false)
							&& !isCheck(2, piece.getColor(), false) && !isCheck(3, piece.getColor(), false) && !isCheck(4, piece.getColor(), false)) {
							moves.push_back(2);
							castle = 2;
						}
					}
				}
			}
			else {
				temp1 = "";
				temp2 = "";
				for (int i = 60; i < 64; i++) {
					temp1 += positions[positions.size() - 1][i];
				}

				if (temp1 == "k00r") {

					if (!selectPiece('R', 63)->getMoved()) {

						if (!isCheck(60, piece.getColor(), false) && !isCheck(61, piece.getColor(), false)
							&& !isCheck(62, piece.getColor(), false) && !isCheck(63, piece.getColor(), false)) {

							moves.push_back(62);
							castle = 62;
						}
					}
				}
				for (int i = 56; i < 61; i++) {
					temp2 += positions[positions.size() - 1][i];
				}
				if (temp2 == "r000k") {
					if (!selectPiece('R', 56)->getMoved()) {
						if (!isCheck(56, piece.getColor(), false) && !isCheck(57, piece.getColor(), false)
							&& !isCheck(58, piece.getColor(), false) && !isCheck(59, piece.getColor(), false) && !isCheck(60, piece.getColor(), false)) {
							moves.push_back(58);
							castle = 58;
						}
					}
				}
			}
		}
		return moves;
	}

	//Pawn movement rules
	// color = true -> white | false -> black
	if (piece.getName() == 'P') {

		if (piece.getColor() == true) {
			if (searchPiece(pos + 8).first == '0' && row > 0) {
				moves.push_back(pos + 8);
				if (row == 6 && searchPiece(pos + 16).first == '0' && row > 1)
					moves.push_back(pos + 16);
			}
			if (searchPiece(pos + 9).first != '0' && searchPiece(pos + 9).second == false && col > 0)
				moves.push_back(pos + 9);
			if (searchPiece(pos + 7).first != '0' && searchPiece(pos + 7).second == false && col < 7)
				moves.push_back(pos + 7);

			//EnPassant white
			if (piece.getSquare() / 8 == 4) {
				if (Move::moves[Move::moves.size() - 1].pawn) {

					if (Move::moves[Move::moves.size() - 1].getFrom() == piece.getSquare() + 17) {
						if (Move::moves[Move::moves.size() - 1].getTo() == piece.getSquare() + 1) {
							moves.push_back(piece.getSquare() + 9);
							enPassant = piece.getSquare() + 9;
						}
					}
					if (Move::moves[Move::moves.size() - 1].getFrom() == piece.getSquare() + 15) {

						if (Move::moves[Move::moves.size() - 1].getTo() == piece.getSquare() - 1) {
							moves.push_back(piece.getSquare() + 7);
							enPassant = piece.getSquare() + 7;
						}
					}
				}
			}
		}
		if (piece.getColor() == false) {
			if (searchPiece(pos - 8).first == '0' && row < 7) {
				moves.push_back(pos - 8);
				if (row == 1 && searchPiece(pos - 16).first == '0' && row < 6)
					moves.push_back(pos - 16);
			}
			if (searchPiece(pos - 9).first != '0' && searchPiece(pos - 9).second == true && col < 7)
				moves.push_back(pos - 9);
			if (searchPiece(pos - 7).first != '0' && searchPiece(pos - 7).second == true && col > 0)
				moves.push_back(pos - 7);
			//EnPassant black
			if (piece.getSquare() / 8 == 3) {
				if (Move::moves[Move::moves.size() - 1].pawn) {
					if (Move::moves[Move::moves.size() - 1].getFrom() == piece.getSquare() - 17) {
						if (Move::moves[Move::moves.size() - 1].getTo() == piece.getSquare() - 1) {
							moves.push_back(piece.getSquare() - 9);
							enPassant = piece.getSquare() - 9;
						}
					}
					if (Move::moves[Move::moves.size() - 1].getFrom() == piece.getSquare() - 15) {
						if (Move::moves[Move::moves.size() - 1].getTo() == piece.getSquare() + 1) {
							moves.push_back(piece.getSquare() - 7);
							enPassant = piece.getSquare() - 7;
						}
					}
				}
			}
		}
	}

	//Rook movement rules
	if (piece.getName() == 'R') {

		for (int i = 1; i <= row; i++) {
			if (searchPiece(pos + 8 * i).first != '0') {
				if (searchPiece(pos + 8 * i).second != piece.getColor()) {
					moves.push_back(pos + 8 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos + 8 * i);
		}
		for (int i = 1; i <= (7 - row); i++) {
			if (searchPiece(pos - 8 * i).first != '0') {
				if (searchPiece(pos - 8 * i).second != piece.getColor()) {
					moves.push_back(pos - 8 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos - 8 * i);
		}
		for (int i = 1; i <= static_cast<int>(col); i++) {
			if (searchPiece(pos + i).first != '0') {
				if (searchPiece(pos + i).second != piece.getColor()) {
					moves.push_back(pos + i);
					break;
				}
				else break;
			}
			moves.push_back(pos + i);
		}
		for (int i = 1; i <= (7 - static_cast<int>(col)); i++) {
			if (searchPiece(pos - i).first != '0') {
				if (searchPiece(pos - i).second != piece.getColor()) {
					moves.push_back(pos - i);
					break;
				}
				else break;
			}
			moves.push_back(pos - i);
		}
	}

	//Knight movement rules
	if (piece.getName() == 'N') {
		if (row > 1) {
			if (col > 0) {
				if (searchPiece(pos + 17).first == '0') moves.push_back(pos + 17);
				if (searchPiece(pos + 17).first != '0' && searchPiece(pos + 17).second != piece.getColor())
					moves.push_back(pos + 17);
			}
			if (col < 7) {
				if (searchPiece(pos + 15).first == '0') moves.push_back(pos + 15);
				if (searchPiece(pos + 15).first != '0' && searchPiece(pos + 15).second != piece.getColor())
					moves.push_back(pos + 15);
			}
		}
		if (row < 6) {
			if (col < 7) {
				if (searchPiece(pos - 17).first == '0') moves.push_back(pos - 17);
				if (searchPiece(pos - 17).first != '0' && searchPiece(pos - 17).second != piece.getColor())
					moves.push_back(pos - 17);
			}
			if (col > 0) {
				if (searchPiece(pos - 15).first == '0') moves.push_back(pos - 15);
				if (searchPiece(pos - 15).first != '0' && searchPiece(pos - 15).second != piece.getColor())
					moves.push_back(pos - 15);
			}
		}
		if (col > 1) {
			if (row > 0) {
				if (searchPiece(pos + 10).first == '0') moves.push_back(pos + 10);
				if (searchPiece(pos + 10).first != '0' && searchPiece(pos + 10).second != piece.getColor())
					moves.push_back(pos + 10);
			}
			if (row < 7) {
				if (searchPiece(pos - 6).first == '0') moves.push_back(pos - 6);
				if (searchPiece(pos - 6).first != '0' && searchPiece(pos - 6).second != piece.getColor())
					moves.push_back(pos - 6);
			}
		}
		if (col < 6) {
			if (row < 7) {
				if (searchPiece(pos - 10).first == '0') moves.push_back(pos - 10);
				if (searchPiece(pos - 10).first != '0' && searchPiece(pos - 10).second != piece.getColor())
					moves.push_back(pos - 10);
			}
			if (row > 0) {
				if (searchPiece(pos + 6).first == '0') moves.push_back(pos + 6);
				if (searchPiece(pos + 6).first != '0' && searchPiece(pos + 6).second != piece.getColor())
					moves.push_back(pos + 6);
			}
		}
	}

	//Bishop movement rules
	if (piece.getName() == 'B') {
		for (int i = 1; i <= std::min(row, static_cast<int>(col)); i++) {
			if (searchPiece(pos + 9 * i).first != '0') {
				if (searchPiece(pos + 9 * i).second != piece.getColor()) {
					moves.push_back(pos + 9 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos + 9 * i);
		}
		for (int i = 1; i <= std::min((7 - row), static_cast<int>(col)); i++) {
			if (searchPiece(pos - 7 * i).first != '0') {
				if (searchPiece(pos - 7 * i).second != piece.getColor()) {
					moves.push_back(pos - 7 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos - 7 * i);
		}
		for (int i = 1; i <= std::min((7 - row), static_cast<int>(7 - col)); i++) {
			if (searchPiece(pos - 9 * i).first != '0') {
				if (searchPiece(pos - 9 * i).second != piece.getColor()) {
					moves.push_back(pos - 9 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos - 9 * i);
		}
		for (int i = 1; i <= std::min(row, static_cast<int>(7 - col)); i++) {
			if (searchPiece(pos + 7 * i).first != '0') {
				if (searchPiece(pos + 7 * i).second != piece.getColor()) {
					moves.push_back(pos + 7 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos + 7 * i);
		}
	}

	//Queen movement rules
	if (piece.getName() == 'Q') {
		for (int i = 1; i <= std::min(row, static_cast<int>(col)); i++) {
			if (searchPiece(pos + 9 * i).first != '0') {
				if (searchPiece(pos + 9 * i).second != piece.getColor()) {
					moves.push_back(pos + 9 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos + 9 * i);
		}
		for (int i = 1; i <= std::min((7 - row), static_cast<int>(col)); i++) {
			if (searchPiece(pos - 7 * i).first != '0') {
				if (searchPiece(pos - 7 * i).second != piece.getColor()) {
					moves.push_back(pos - 7 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos - 7 * i);
		}
		for (int i = 1; i <= std::min((7 - row), static_cast<int>(7 - col)); i++) {
			if (searchPiece(pos - 9 * i).first != '0') {
				if (searchPiece(pos - 9 * i).second != piece.getColor()) {
					moves.push_back(pos - 9 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos - 9 * i);
		}
		for (int i = 1; i <= std::min(row, static_cast<int>(7 - col)); i++) {
			if (searchPiece(pos + 7 * i).first != '0') {
				if (searchPiece(pos + 7 * i).second != piece.getColor()) {
					moves.push_back(pos + 7 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos + 7 * i);
		}


		for (int i = 1; i <= row; i++) {
			if (searchPiece(pos + 8 * i).first != '0') {
				if (searchPiece(pos + 8 * i).second != piece.getColor()) {
					moves.push_back(pos + 8 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos + 8 * i);
		}
		for (int i = 1; i <= (7 - row); i++) {
			if (searchPiece(pos - 8 * i).first != '0') {
				if (searchPiece(pos - 8 * i).second != piece.getColor()) {
					moves.push_back(pos - 8 * i);
					break;
				}
				else break;
			}
			moves.push_back(pos - 8 * i);
		}
		for (int i = 1; i <= static_cast<int>(col); i++) {
			if (searchPiece(pos + i).first != '0') {
				if (searchPiece(pos + i).second != piece.getColor()) {
					moves.push_back(pos + i);
					break;
				}
				else break;
			}
			moves.push_back(pos + i);
		}
		for (int i = 1; i <= (7 - static_cast<int>(col)); i++) {
			if (searchPiece(pos - i).first != '0') {
				if (searchPiece(pos - i).second != piece.getColor()) {
					moves.push_back(pos - i);
					break;
				}
				else break;
			}
			moves.push_back(pos - i);
		}
	}

	//Trimming available moves in case of piece being pinned to the king


	int king = -1;

	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getName() == 'K' && pieces[i].getColor() == piece.getColor()) {
			king = pieces[i].getSquare();
			break;
		}
	}
	if (king == -1) return moves;

	//King's row and column numbers


	int kingRow = king / 8;
	double kingCol = king / 8.0;
	kingCol = round((kingCol - kingRow) * 10 / 1.25);

	row = pos / 8;
	col = pos / 8.0;
	col = round((col - row) * 10 / 1.25);


	if (row == kingRow) {
		if (piece.getSquare() > king) {

			for (int i = piece.getSquare() - 1; i > king; i--) {
				if (searchPiece(i).first != '0') return moves;
			}

			for (int i = piece.getSquare() + 1; i <= ((0.875 + row) * 8); i++) {
				std::pair<char, bool> temp;
				temp = searchPiece(i);
				if (temp.first != '0') {
					if ((temp.first == 'R' || temp.first == 'Q') && temp.second != piece.getColor()) {
						std::vector<int> moves2;
						for (int j = 0; j < moves.size(); j++) {
							if (moves[j] / 8 == row) moves2.push_back(moves[j]);
						}
						return moves2;
					}
					else break;
				}
			}
		}
		if (piece.getSquare() < king) {

			for (int i = piece.getSquare() + 1; i < king; i++) {
				if (searchPiece(i).first != '0') return moves;
			}

			for (int i = piece.getSquare() - 1; i >= (row * 8); i--) {
				std::pair<char, bool> temp;
				temp = searchPiece(i);
				if (temp.first != '0') {
					if ((temp.first == 'R' || temp.first == 'Q') && temp.second != piece.getColor()) {
						std::vector<int> moves2;
						for (int j = 0; j < moves.size(); j++) {
							if (moves[j] / 8 == row) moves2.push_back(moves[j]);
						}
						return moves2;
					}
					else break;
				}
			}
		}
	}
	if (col == kingCol) {
		if (piece.getSquare() >= king) {

			for (int i = piece.getSquare() - 8; i > king; i -= 8) {
				if (searchPiece(i).first != '0') return moves;
			}

			for (int i = piece.getSquare() + 8; i <= ((7 + col * 0.125) * 8); i += 8) {
				std::pair<char, bool> temp;
				temp = searchPiece(i);
				if (temp.first != '0') {
					if ((temp.first == 'R' || temp.first == 'Q') && temp.second != piece.getColor()) {
						std::vector<int> moves2;
						for (int j = 0; j < moves.size(); j++) {
							if ((moves[j] / 8.0 - (moves[j] / 8)) / 0.125 == col) moves2.push_back(moves[j]);
						}
						return moves2;
					}
					else break;
				}
			}

		}
		if (piece.getSquare() < king) {

			for (int i = piece.getSquare() + 8; i < king; i += 8) {
				if (searchPiece(i).first != '0') return moves;
			}

			for (int i = piece.getSquare() - 8; i >= col; i -= 8) {
				std::pair<char, bool> temp;
				temp = searchPiece(i);
				if (temp.first != '0') {
					if ((temp.first == 'R' || temp.first == 'Q') && temp.second != piece.getColor()) {
						std::vector<int> moves2;
						for (int j = 0; j < moves.size(); j++) {
							if ((moves[j] / 8.0 - (moves[j] / 8)) / 0.125 == col) moves2.push_back(moves[j]);
						}
						return moves2;
					}
					else break;
				}
			}

		}
	}
	if (row - kingRow == static_cast<int>(col - kingCol)) {
		if (piece.getSquare() > king) {

			if (std::abs(piece.getSquare() - king) % 9 == 0) {
				for (int i = 1; i < std::min(row, static_cast<int>(col)) - std::min(kingRow, static_cast<int>(kingCol)); i++) {
					if (searchPiece(piece.getSquare() - 9 * i).first != '0') return moves;
				}

				for (int i = 1; i <= 7 - std::max(row, static_cast<int>(col)); i++) {
					std::pair<char, bool> temp;
					temp = searchPiece(piece.getSquare() + 9 * i);
					if (temp.first != '0') {
						if ((temp.first == 'B' || temp.first == 'Q') && temp.second != piece.getColor()) {
							std::vector<int> moves2;
							for (int j = 0; j < moves.size(); j++) {
								if ((moves[j] - piece.getSquare()) % 9 == 0) moves2.push_back(moves[j]);
							}
							return moves2;
						}
						else break;
					}
				}
			}

			if (std::abs(piece.getSquare() - king) % 7 == 0) {

				for (int i = piece.getSquare() - 7; i > king; i -= 7) {
					if (searchPiece(i).first != '0') return moves;
				}

				for (int i = piece.getSquare() + 7; i <= (std::min(7 - row, static_cast<int>(col))) * 7 + piece.getSquare(); i += 7) {
					std::pair<char, bool> temp;
					temp = searchPiece(i);
					if (temp.first != '0') {
						if ((temp.first == 'B' || temp.first == 'Q') && temp.second != piece.getColor()) {
							std::vector<int> moves2;
							for (int j = 0; j < moves.size(); j++) {
								if ((moves[j] - piece.getSquare()) % 7 == 0) moves2.push_back(moves[j]);
							}
							return moves2;
						}
						else break;
					}
				}
			}
		}


		if (piece.getSquare() < king) {

			if (std::abs(piece.getSquare() - king) % 9 == 0) {

				for (int i = 1; i < (7 - std::max(row, static_cast<int>(col))) - (7 - std::max(kingRow, static_cast<int>(kingCol))); i++) {
					if (searchPiece(piece.getSquare() + 9 * i).first != '0') return moves;
				}

				for (int i = 1; i <= std::min(row, static_cast<int>(col)); i++) {
					std::pair<char, bool> temp;
					temp = searchPiece(piece.getSquare() - 9 * i);
					if (temp.first != '0') {
						if ((temp.first == 'B' || temp.first == 'Q') && temp.second != piece.getColor()) {
							std::vector<int> moves2;
							for (int j = 0; j < moves.size(); j++) {
								if ((moves[j] - piece.getSquare()) % 9 == 0) moves2.push_back(moves[j]);
							}
							return moves2;
						}
						else break;
					}
				}
			}

			if (std::abs(piece.getSquare() - king) % 7 == 0) {

				for (int i = piece.getSquare() + 7; i < king; i += 7) {
					if (searchPiece(i).first != '0') return moves;
				}

				for (int i = piece.getSquare() - 7; i >= (piece.getSquare() - std::min(row, 7 - static_cast<int>(col)) * 7); i -= 7) {
					std::pair<char, bool> temp;
					temp = searchPiece(i);
					if (temp.first != '0') {
						if ((temp.first == 'B' || temp.first == 'Q') && temp.second != piece.getColor()) {
							std::vector<int> moves2;
							for (int j = 0; j < moves.size(); j++) {
								if ((moves[j] - piece.getSquare()) % 7 == 0) moves2.push_back(moves[j]);
							}
							return moves2;
						}
						else break;
					}
				}
			}
		}
	}
	return moves;
}


std::vector<int> Board::availableMovesCheck(Piece& piece, int pos) {
	if (piece.getName() == 'K')
		return availableMoves(piece, pos);

	std::vector<int> moves;

	int kingPos = -1;
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getName() == 'K' && pieces[i].getColor() == piece.getColor()) {
			kingPos = pieces[i].getSquare();
			break;
		}
	}

	std::vector<int> path;						//Path between the checker piece and the King


	int kingRow = kingPos / 8;					//The row number in which the king is at
	double kingCol = kingPos / 8.0;				//The column number in which the king is at
	kingCol = round((kingCol - kingRow) * 10 / 1.25);

	int checkerRow = checker / 8;				//The row number in which the checker piece is at
	double checkerCol = checker / 8.0;			//The column number in which the checker piece is at
	checkerCol = round((checkerCol - checkerRow) * 10 / 1.25);

	if (kingRow == checkerRow) {
		for (int i = std::min(kingPos, checker) + 1; i <= std::max(kingPos, checker); i++) {
			path.push_back(i);
		}
	}
	else if (kingCol == checkerCol) {
		for (int i = (std::min(kingPos, checker) + 8); i <= std::max(kingPos, checker); i += 8) {
			path.push_back(i);
		}
	}
	else {
		int identifier = std::abs(kingPos - checker);
		if (identifier % 9 == 0) {
			for (int i = (std::min(kingPos, checker)); i <= std::max(kingPos, checker); i += 9) {
				path.push_back(i);
			}
		}
		if (identifier % 7 == 0) {
			for (int i = (std::min(kingPos, checker)); i <= std::max(kingPos, checker); i += 7) {
				path.push_back(i);
			}
		}
	}

	std::vector<int> available = availableMoves(piece, pos);
	for (int i = 0; i < available.size(); i++) {
		if (std::binary_search(path.begin(), path.end(), available[i])) {
			moves.push_back(available[i]);
		}
		if (available[i] == checker) moves.push_back(checker);
	}
	if (piece.getName() == 'P') {
		for (int i = 0; i < moves.size(); i++) {
			if ((moves[i] - piece.getSquare()) % 8 == 0) {
				if (moves[i] == piece.getSquare()) moves.erase(moves.begin() + i);
			}
		}
	}
	return moves;
}

void Board::recordPos() {			//Record the current board position
	std::string temp = "";
	for (int i = 0; i < 64; i++) {
		temp += '0';
	}

	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getColor() == true) {
			temp[pieces[i].getSquare()] = pieces[i].getName();
		}
		else {
			temp[pieces[i].getSquare()] = pieces[i].getName() + 32;
		}
	}
	positions.push_back(temp);
}

bool Board::isCheckmate(Piece& King) {
	if (isCheck(King.getSquare(), King.getColor(), true)) {
		if (availableMovesCheck(King, King.getSquare()).size() == 0) {
			bool found = false;
			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i].getColor() == King.getColor() && availableMovesCheck(pieces[i], pieces[i].getSquare()).size() > 0) {
					found = true;
					break;
				}
			}
			if (!found) return true;
		}
	}
	return false;
}

bool Board::isDraw(bool color) {
	//three fold repition and perpetual check
	for (int i = positions.size() - 1; i >= 0; i--) {
		int counter = 0;
		for (int j = positions.size() - 1; j >= 0; j--) {
			if (positions[i] == positions[j]) counter++;
			if (counter >= 3) return true;
		}
	}

	//stalemate
	int kingPos = -1;
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getColor() == color && pieces[i].getName() == 'K') {
			kingPos = pieces[i].getSquare();
			break;
		}
	}
	if (!isCheck(kingPos, color, true)) {
		bool found = false;
		for (int i = 0; i < pieces.size(); i++) {
			if (pieces[i].getColor() == color) {
				if (availableMoves(pieces[i], pieces[i].getSquare()).size() > 0) {
					found = true;
					break;
				}
			}
		}
		if (!found) return true;
	}

	//50-move rule
	if (Move::moves.size() >= 100) {
		int counter = 0;
		for (int i = Move::moves.size() - 1; i >= 0; i--) {
			if (counter == 100) return true;
			if (!Move::moves[i].capture && !Move::moves[i].pawn) counter++;
			if (Move::moves[i].capture || Move::moves[i].pawn) {
				counter = 0;
				break;
			}
		}
	}

	//insufficient checkmating material

	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getName() == 'P' || pieces[i].getName() == 'Q' || pieces[i].getName() == 'R') {
			return false;
		}
	}
	if (pieces.size() == 4) {
		int white = 0, black = 0;
		for (int i = 0; i < pieces.size(); i++) {
			if (pieces[i].getColor()) white++;
			if (!pieces[i].getColor()) black++;
		}
		if (white == black) return true;
	}
	if (pieces.size() < 4) return true;
	return false;
}

bool Board::pushMove(int to, Piece* piece) {
	//False-returning conditions

	if (piece->getColor() != whiteTurn) {
		return false;
	}

	// information about the moved piece --> this is for when using erase() to retrieve information about the piece before the pointer changes
	char pieceName = piece->getName();

	Piece* king = NULL;
	bool found = false;
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getName() == 'K' && pieces[i].getColor() == piece->getColor()) {
			king = &pieces[i];
			break;
		}
	}
	if (king != NULL && isCheck(king->getSquare(), king->getColor(), true)) {
		if (availableMovesCheck(*piece, piece->getSquare()).size() == 0) return false;
		bool found = false;
		for (int i = 0; i < availableMovesCheck(*piece, piece->getSquare()).size(); i++) {
			if (availableMovesCheck(*piece, piece->getSquare())[i] == to) {
				found = true;
				break;
			}
		}
		if (!found) return false;
	}
	else if (king != NULL && availableMoves(*piece, piece->getSquare()).size() == 0) return false;
	else {
		for (int i = 0; i < availableMoves(*piece, piece->getSquare()).size(); i++) {
			if (availableMoves(*piece, piece->getSquare())[i] == to) {
				found = true;
				break;
			}
		}
		if (!found) return false;
	}

	//King's special moves, if piece = king
	if (piece->getName() == 'K' && std::abs(piece->getSquare() - to) == 2) {
		if (searchPiece(to + 1).first == 'R') {
			selectPiece('R', to + 1)->setSquare(to - 1);
		}
		if (searchPiece(to - 2).first == 'R') {
			selectPiece('R', to - 2)->setSquare(to + 1);
		}
	}

	king = NULL;


	int from = piece->getSquare();
	Move move;
	move.setFrom(from);
	move.setTo(to);
	if (piece->getName() == 'P') move.pawn = true;

	piece->setSquare(to);

	found = false;
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getSquare() == to && pieces[i].getColor() != piece->getColor()) {
			found = true;
			captured.push_back(pieces[i]);
			pieces.erase(pieces.begin() + i);
			break;
		}
	}

	if (found) {
		move.capture = true;
	}

	if (to == enPassant) {
		move.capture = true;
		if (whiteTurn) {
			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i].getSquare() == (to - 8)) {
					captured.push_back(pieces[i]);
					pieces.erase(pieces.begin() + i);
					break;
				}
			}
		}
		if (!whiteTurn) {
			for (int i = 0; i < pieces.size(); i++) {
				if (pieces[i].getSquare() == (to + 8)) {
					captured.push_back(pieces[i]);
					pieces.erase(pieces.begin() + i);
					break;
				}
			}
		}
	}

	Move::moves.push_back(move);
	recordPos();

	//after erase(), redirect the pointer towards the originally selected piece
	piece = selectPiece(pieceName, to);


	//search for check
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i].getName() == 'K' && pieces[i].getColor() != piece->getColor()) {
			king = &pieces[i];
			break;
		}
	}
	if (king != NULL) {
		checkmate = isCheckmate(*king);
		check = isCheck(king->getSquare(), king->getColor(), true);
		draw = isDraw(king->getColor());
	}


	enPassant = -1;			//Reset enpassant square
	castle = -1;
	if (whiteTurn) whiteTurn = false;
	else whiteTurn = true;

	return true;
}

