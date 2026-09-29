Chesser is an online chess game where players can log into their accounts or register new ones, queue against each other then play a game of chess.
The project is simple and can be scaled.
The project file contains the back-end and the front-end files.



- Account Management => ASP.net servers that handles endpoints such as logins and registers. It contains a localDB whose purpose is to test the logic of the database itself, this can be replaced with an SQL server for scalability.
It can also be modified a little bit to ensure secure endpoint reach.

- ChessLib => This contains the C++ source code of the game of chess, includes all the rules and verifies moves.

- GameServer => This contains the C++ backend game server files, whose purpose is to verify moves and send updates to the front ends. This file contains the ChessLib library and some external open source libraries that help parse JSON data and send/receive TCP messages.

- Logic Testing => This contains a simple console interface where moves are tested to insure no bugs are in the source code. The GameServer file contains a .txt file which records all the moves that the front-end sides sends, this can be used inside the Logic Testing in case of a bug for easier detection.

- Web chess board => This file contains the front-end, designed with vanilla Javascript. It contains the main screen of login and register, and the mainboard where players queue.


Project Flow:
- Once the player registers an account and/or logs in, the ASP.net server generates access token and sends it to the client where it's saved in the client's internal storage. This token is sent with each request that the client sends to the server
  to verify if the user is logged in or not, the token has a hashed header and a body that contains the user's information, the server checks the header if it matches so that it could safely access the body. This is so that the server doesn't have to save the token in the database after each login.
- When the player presses the logout button, the token is simply removed from the user's localstorage and from the database, which will require the user to login again so that another token is generated.
- When the player wants to find a lobby/queue for a match, the asp.net server authorizes the player by evaluating the accesstoken. If it is successfully verified, the server generates a ticket and sends the ticket to the user client, then sends the ticket alongside the user id and username to the C++ game server.
  The game server saves the information sent by the ASP.net server, then the user establishes a direct TCP connection to the C++ server. Upon connection, the user sends the ticket that the ASP.net server sent, to the game server, then the game server verifies the ticket and saves the user to the queue.
- If two players are in the queue, the queue empties itself and the game server generates a new lobby, then generates a thread that sends an HTTP request to the asp.net server to have it create a new lobby in the database, the game then begins and at the end of the match, the results are stored in the database.

Important notes:
- The server handles disconnections, but the project currently doesn't have an in-game timer, which means the lobby will remain forever until the user reconnects back again, this could be resolved by simply adding an in-game timer, if the timer runs out, the other user is declared the winner.
- The internal communication between both servers can be made more secure to prevent outsiders from accessing the internal endpoint.
- The database is made using localDB. To handle big number of players, the database needs to be updated to an SQL server.
- Future additions that I'll add on this project in the future includes:
  a. A load balancer.
  b. A bot.
  c. A backup server in case of server going down.
