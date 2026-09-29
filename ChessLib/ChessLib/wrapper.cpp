#include <emscripten/bind.h>
#include "logic.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(module) {

    register_vector<Move>("moveVector");
    register_vector<int>("intVector");
    register_vector<Piece>("pieceVector");
    register_vector<std::string>("stringVector");

    class_<Piece>("Piece").constructor<int, char, bool>()
    .function("setSquare", &Piece::setSquare)
    .function("getSquare", &Piece::getSquare)
    .function("getName", &Piece::getName)
    .function("getColor", &Piece::getColor)
    .function("getMoved", &Piece::getMoved)
    .function("Promote", &Piece::promote, allow_raw_pointers());

    class_<Move>("Move").constructor<>()
    .property("pawn", &Move::pawn)
	.property("capture", &Move::capture)
	.function("setFrom", &Move::setFrom)
	.function("setTo", &Move::setTo)
	.function("getFrom", &Move::getFrom)
	.function("getTo", &Move::getTo)
	.class_property("moves", &Move::moves);


    class_<Board>("Board").constructor<>().class_function("createWithParams", &Board::createWithParams)
    .function("selectPiece", &Board::selectPiece, allow_raw_pointers())
    .function("getCastle", &Board::getCastle)
    .function("getEnPassant", &Board::getEnPassant)
    .function("getCheckMate", &Board::getCheckmate)
    .function("getCheck", &Board::getCheck)
    .function("getDraw", &Board::getDraw)
    .function("getTurn", &Board::getTurn)
    .function("pushMove", &Board::pushMove, allow_raw_pointers())
    .function("availableMoves", &Board::availableMoves, allow_raw_pointers())
    .function("availableMovesCheck", &Board::availableMovesCheck, allow_raw_pointers())
    .property("pieces", &Board::pieces)
	.property("captured", &Board::captured);

}