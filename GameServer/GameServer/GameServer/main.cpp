#include "Lobby.h"
#include <ixwebsocket/IXHttpServer.h>
#include <string>
#include <deque>
#include <unordered_map>
#include <mutex>


using json = nlohmann::json;

std::mutex mutex;
std::deque<Player> Queue;
std::unordered_map<int, std::shared_ptr<Lobby>> activeLobbies;
std::unordered_map<ix::WebSocket*, int> socketToPlayer;

std::unordered_map<int, ix::WebSocket*> playerToSocket;

std::unordered_map<std::string, std::pair<int, std::string>> expectedPlayers;

int Lobby::counter = 0;


int main() {

	ix::initNetSystem();
	ix::WebSocketServer server(8080, "0.0.0.0");



	server.setOnClientMessageCallback([](
		std::shared_ptr<ix::ConnectionState> connectionState,
		ix::WebSocket& webSocket,
		const ix::WebSocketMessagePtr& msg) {
			if (msg->type == ix::WebSocketMessageType::Open) {


				std::cout << "Connection success" << std::endl;

			}
			if (msg->type == ix::WebSocketMessageType::Close) {
				std::lock_guard<std::mutex> lock(mutex);


				auto it = socketToPlayer.find(&webSocket);

				if (it != socketToPlayer.end()) {
					int playerID = it->second;


					socketToPlayer.erase(it);
					playerToSocket.erase(playerID);

				}
			}
			if (msg->type == ix::WebSocketMessageType::Error) {

			}
			if (msg->type == ix::WebSocketMessageType::Message) {

				try {
					json message = json::parse(msg->str);


					if (message["type"].get<std::string>() == "auth") {
						std::string ticket = message["body"]["ticket"].get<std::string>();

						std::lock_guard<std::mutex> lock(mutex);

						auto it = expectedPlayers.find(ticket);

						if (it != expectedPlayers.end()) {		//Player authenticated successfully
							int playerID = it->second.first;
							std::string username = it->second.second;

							if (playerToSocket.count(playerID) > 0) {
								auto oldSocket = playerToSocket[playerID];
								socketToPlayer.erase(oldSocket);
								oldSocket->close();
							}

							socketToPlayer.insert({ &webSocket, playerID });
							playerToSocket[playerID] = &webSocket;

							auto it2 = activeLobbies.find(playerID);

							if (it2 != activeLobbies.end()) {
								//reconnecting player
								it2->second->reconnectPlayer(playerID, &webSocket);
							}
							else {
								//new player
								Player player;
								player.ID = playerID;
								player.username = username;
								player.socket = &webSocket;

								Queue.push_back(player);

								if (Queue.size() >= 2) {
									Player p1 = Queue[0];
									Player p2 = Queue[1];

									Queue.pop_front();
									Queue.pop_front();

									auto newGame = std::make_shared<Lobby>(p1, p2);
									activeLobbies[p1.ID] = newGame;
									activeLobbies[p2.ID] = newGame;

									newGame->startGame();


									//Send lobby info to ASP.net
									json payload = {
										{"HostPlayerID", p1.ID},
										{"SecPlayerID", p2.ID},
										{"LobbyID", newGame->getID()}
									};
									std::string endpoint = "/api/internal/createlobby";
									//newGame->updateDatabase(endpoint, payload);
								}
							}

							expectedPlayers.erase(it);
						}
						else {
							webSocket.close();
						}
					}

					if (message["type"].get<std::string>() == "cancel_search") {
						for (int i = 0; i < Queue.size(); i++) {
							if (&webSocket == Queue[i].socket) {
								Queue.erase(Queue.begin() + i);
							}
						}
					}

					if (message["type"].get<std::string>() == "move") {
						int playerID = socketToPlayer[&webSocket];
						auto lobby = activeLobbies[playerID];
						lobby->receiveMessage(&webSocket, message);
					}

					if (message["type"].get<std::string>() == "resign") {

					}

					if (message["type"].get<std::string>() == "offer_draw") {

					}

					if (message["type"].get<std::string>() == "draw_response") {

					}
					if (message["type"].get<std::string>() == "CLOSE") {
						int playerID = socketToPlayer[&webSocket];
						activeLobbies.erase(playerID);
					}

				}
				catch (const json::exception& e) {
					std::cerr << "JSON Error: " << e.what() << std::endl;
				}
			}
		});

	auto res = server.listen();
	if (!res.first) {
		std::cerr << "[SERVER] Failed to start on port 8080. Error: " << res.second << "\n";
		return 1;
	}

	server.start();

	ix::HttpServer httpServer(8090, "127.0.0.1");		//asp.net server connection

	httpServer.setOnConnectionCallback([](ix::HttpRequestPtr request,
		std::shared_ptr<ix::ConnectionState> connectionState) -> ix::HttpResponsePtr {
			if (request->uri == "/internal/issue-ticket" && request->method == "POST") {
				try {

					//Uncork the message sent by the ASP.net server
					
					json message = json::parse(request->body);
					std::string username = message["Username"];
					int playerID = message["PlayerID"];
					std::string ticket = message["Ticket"];

					mutex.lock();
					expectedPlayers.insert({ticket, {playerID, username}});

					std::cout << ticket << std::endl << playerID << std::endl << username << std::endl;
					mutex.unlock();

					return std::make_shared<ix::HttpResponse>(200, "OK");

				}
				catch (const json::exception& e) {
					std::cerr << "JSON Error: " << e.what() << std::endl;
					return std::make_shared<ix::HttpResponse>(400, "Bad Request");
				}
				return std::make_shared<ix::HttpResponse>(404, "Not Found");
			}

		});

	auto response = httpServer.listen();
	if (response.first) {
		httpServer.start();
		std::cout << "Internal HTTP Server running on port 8090..." << std::endl;
	}
	else {
		std::cerr << "HTTP Server failed to start: " << response.second << std::endl;
	}

	httpServer.wait();
	server.wait();



	return 0;
}