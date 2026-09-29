#include "Lobby.h"

Player::~Player() {
	socket = NULL;
}

Lobby::Lobby(Player& player1, Player& player2) {
	white = player1;
	black = player2;
	counter++;
	LobbyID = counter;
}

void Lobby::testDoc(std::string command) {
	std::ofstream File("debug.txt", std::ios::app);
	if (!File.is_open()) {
		return;
	}
	File << "1 " << command;
}


void Lobby::startGame() {
	if (white.socket != NULL && black.socket != NULL) {
		json message = {
			{"type", "startgame"},
			{"body", {{"color", "white"}, {"lastpos", getLastPos()}, {"turn", "true"}}}
		};
		white.socket->send(message.dump());
		message = {
			{"type", "startgame"},
			{"body", {{"color", "black"}, {"lastpos", getLastPos()}, {"turn", "true"}}}
		};
		black.socket->send(message.dump());
	}
}

void Lobby::receiveMessage(ix::WebSocket* socket, json& instr) {
	
	Player* sender = NULL;
	Player* receiver = NULL;
	if (socket == white.socket) {
		sender = &white;
		receiver = &black;
	}
	else {
		sender = &black;
		receiver = &white;
	}


	if (instr["type"] == "move") {
		char name = std::stoi(instr["body"]["piece"].get<std::string>());
		int from = std::stoi(instr["body"]["from"].get<std::string>());
		int to = std::stoi(instr["body"]["to"].get<std::string>());
		piece = board.selectPiece(name, from);
		if (board.pushMove(to, piece)) {
			receiver->socket->send(instr.dump());

			std::string piecename(1, name);
			std::string xd = piecename + " " + std::to_string(from) + " " + std::to_string(to) + " ";
			testDoc(xd);

			if (board.getCheckmate()) {
				std::string temp = "";
				if (board.getTurn() == false) temp = "true";
				else temp = "false";
				json message = {
					{"type", "checkmate"},
					{"body", {{"winner", temp}}}
				};
				sender->socket->send(message.dump());
				receiver->socket -> send(message.dump());

				std::string endpoint = "/api/internal/updatelobby";
				int winnerID = -1;
				int loserID = -1;
				if (temp == "true") {
					winnerID = white.ID;
					loserID = black.ID;
				}
				if (temp == "false") {
					winnerID = black.ID;
					loserID = white.ID;
				}
				json payload = {
					{"LobbyID", this->getID()},
					{"WinnerPlayerID", winnerID}
				};
				//updateDatabase(endpoint, payload);

				endpoint = "/api/internal/updateplayer";
				payload = {
					{"PlayerID", winnerID},
					{"Wins", 1},
					{"Losses", 0},
					{"Draws", 0}
				};
				updateDatabase(endpoint, payload);
				payload = {
					{"PlayerID", loserID},
					{"Wins", 0},
					{"Losses", 1},
					{"Draws", 0}
				};
				updateDatabase(endpoint, payload);

			}
			if (board.getDraw()) {
				json message = {
					{"type", "draw"},
				};

				sender->socket->send(message.dump());
				receiver->socket->send(message.dump());

				std::string endpoint = "api/internal/updatelobby";

				json payload = {
					{"LobbyID", this->getID()},
					{"WinnerPlayerID", -1}
				};
				updateDatabase(endpoint, payload);

				endpoint = "api/internal/updateplayer";
				payload = {
					{"PlayerID", white.ID},
					{"Wins", 0},
					{"Losses", 0},
					{"Draws", 1}
				};
				updateDatabase(endpoint, payload);

				payload = {
					{"PlayerID", black.ID},
					{"Wins", 0},
					{"Losses", 0},
					{"Draws", 1}
				};

				updateDatabase(endpoint, payload);
			}
		}
		else {
			sender->socket->send("ERROR INVALD MOVE");
		}
		
	}
	if (instr["type"] == "resign") {
		winner = receiver;
		receiver->socket->send("WINNER");
	}
	if (instr["type"] == "offer_draw") {
		receiver->socket->send("DRAW?");
	}
	if (instr["type"] == "accept_draw") {
		Draw = true;
		sender->socket->send("DRAW");
		receiver->socket->send("DRAW");
	}
}

int Lobby::getID() { return LobbyID; }


std::string Lobby::getLastPos() {
	return board.getLastPosition();
}

void Lobby::reconnectPlayer(int playerID, ix::WebSocket* socket) {
	std::string turn = "";
	if (board.getTurn() == true) turn = "true";
	else turn = "false";
	if (white.ID == playerID) {
		white.socket = socket;
		white.isConnected = true;
		json message = {
			{"type", "startgame"},
			{"body", {{"lastpos", getLastPos()}, {"color", "white"}, {"turn", turn}}}
		};
		white.socket->send(message.dump());
	}
	else {
		black.socket = socket;
		black.isConnected = true;
		json message = {
			{"type", "startgame"},
			{"body", {{"lastpos", getLastPos()}, {"color", "black"}, {"turn", turn}}}
		};
		black.socket->send(message.dump());
	}
}

void Lobby::updateDatabase(std::string& apiEndpoint, json& payload) {
	std::string jsonString = payload.dump();

	std::string fullUrl = "http://localhost:5084" + apiEndpoint;

	std::thread([fullUrl, jsonString]() {
		ix::HttpClient httpClient;
		auto args = httpClient.createRequest();

		args->extraHeaders["Content-Type"] = "application/json";

		auto response = httpClient.post(fullUrl, jsonString, args);


		if (response->statusCode == 200 || response->statusCode == 204) {
			std::cout << "ASP.NET Updated -> " << fullUrl << std::endl;
		}
		else {
			std::cerr << "ASP.NET Update Failed! Status: " << response->statusCode
				<< " Message: " << response->errorMsg << std::endl;
		}
		}).detach();
}

void Lobby::endGame(std::string winner, bool draw) {

}