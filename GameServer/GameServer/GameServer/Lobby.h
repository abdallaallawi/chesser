#pragma once
#include <iostream>
#include <ixwebsocket/IXWebSocketServer.h>
#include <ixwebsocket/IXHttpClient.h>
#include <logic.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>

using json = nlohmann::json;


class Player {
public:
	std::string username;
	int ID;
	ix::WebSocket* socket = NULL;
	bool color = true;
	bool isConnected = true;
	~Player();
};

class Lobby {
	int LobbyID;
	static int counter;
	Player white;
	Player black;
	Player* winner = NULL;
	bool Draw = false;
	Board board;
	Piece* piece = NULL;
	void testDoc(std::string command);

public:
	Lobby(Player& player1, Player& player2);
	int getID();
	void startGame();
	void receiveMessage(ix::WebSocket* socket, json& instr);
	std::string getLastPos();
	void reconnectPlayer(int playerID, ix::WebSocket* socket);
	void endGame(std::string winner, bool draw = false);
	void updateDatabase(std::string& apiEndpoint, json& payload);
};