#include "logic.h"
using namespace std;


int main() {
	int choice = 0;
	char value;
	//cout << "How many pieces?" << endl;
	//cin >> choice;
	//int index = 0;
	//std::vector<Piece> pieces;
	//while (index < choice) {
	//	char name;
	//	bool color;
	//	int pos = -1;
	//	cout << "Piece type (character): ";
	//	cin >> name;
	//	cout << "Piece color (white = 1, black = 0): ";
	//	cin >> color;
	//	cout << "Position: ";
	//	cin >> pos;
	//	pieces.push_back(Piece(pos, name, color));
	//	index++;
	//}
	//Board board(pieces);
	Board board;
	Piece* piece = NULL;
	while (!board.getCheckmate() || !board.getDraw()) {
		if (board.getTurn()) {
			cout << "White turn:" << endl;
			cout << "1. Select a piece (char and position)" << endl;
			cout << "2. Show available moves for a piece (char and position)" << endl;
			cout << "3. Move a piece. (destination square)" << endl;
			cin >> choice;
			switch (choice) {
			case 1:
				cout << "Type character and position: ";
				cin >> value;
				cin >> choice;
				piece = board.selectPiece(value, choice);
				if (piece != NULL) {
					if (piece->getColor())
						cout << piece->getName() + " selected" << endl;
					else {
						cout << "Can't select black pieces";
						return 0;
					}
				}
				else {
					cout << "Issue with piece selection.";
					return 0;
				}
			case 2:
				if (piece != NULL) {
					if (board.getCheck()) {
						for (int i = 0; i < board.availableMovesCheck(*piece, piece->getSquare()).size(); i++) {
							cout << board.availableMovesCheck(*piece, piece->getSquare())[i] << " ";
						}
						cout << endl;
					}
					else {
						for (int i = 0; i < board.availableMoves(*piece, piece->getSquare()).size(); i++) {
							cout << board.availableMoves(*piece, piece->getSquare())[i] << " ";
						}
						cout << endl;
					}
				}
			case 3:
				cout << "Select a destination: ";
				cin >> choice;
				if (!board.pushMove(choice, piece)) {
					cout << "Invalid move";
					return 0;
				}
				else {
					cout << piece->getName() << Move::moves[Move::moves.size() - 1].getFrom() << " was moved to " << Move::moves[Move::moves.size() - 1].getTo()
						<< endl;
					cout << "Captured Pieces: ";
					for (int i = 0; i < board.captured.size(); i++) {
						cout << board.captured[i].getName() << " ";
					}
					if (board.getCheckmate()) return 0;
					if (board.getDraw()) return 0;
					piece = NULL;
				}
			}
		}
		else {
			cout << "Black turn:" << endl;
			cout << "1. Select a piece (char and position)" << endl;
			cout << "2. Show available moves for a piece (char and position)" << endl;
			cout << "3. Move a piece. (destination square)" << endl;
			cin >> choice;
			switch (choice) {
			case 1:
				cout << "Type character and position: ";
				cin >> value;
				cin >> choice;
				piece = board.selectPiece(value, choice);
				if (piece != NULL) {
					if (!piece->getColor())
						cout << piece->getName() + " selected" << endl;
					else {
						cout << "Can't select white pieces";
						return 0;
					}
				}
				else {
					cout << "Issue with piece selection.";
					return 0;
				}
			case 2:
				if (piece != NULL) {
					if (board.getCheck()) {
						for (int i = 0; i < board.availableMovesCheck(*piece, piece->getSquare()).size(); i++) {
							cout << board.availableMovesCheck(*piece, piece->getSquare())[i] << " ";
						}
						cout << endl;
					}
					else {
						for (int i = 0; i < board.availableMoves(*piece, piece->getSquare()).size(); i++) {
							cout << board.availableMoves(*piece, piece->getSquare())[i] << " ";
						}
						cout << endl;
					}
				}
			case 3:
				cout << "Select a destination: ";
				cin >> choice;
				if (!board.pushMove(choice, piece)) {
					cout << "Invalid move";
					return 0;
				}
				else {
					cout << piece->getName() << Move::moves[Move::moves.size() - 1].getFrom() << " was moved to " << Move::moves[Move::moves.size() - 1].getTo()
						<< endl;
					cout << "Captured Pieces: ";
					for (int i = 0; i < board.captured.size(); i++) {
						cout << board.captured[i].getName() << " ";
					}
					if (board.getCheckmate()) return 0;
					if (board.getDraw()) return 0;
					piece = NULL;
				}
				break;
			}
		}

	}


	return 0;
}