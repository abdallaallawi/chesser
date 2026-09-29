using Microsoft.AspNetCore.Mvc;
using AccountManagement.DTOs;
using AccountManagement.Data;
using AccountManagement.Models;

namespace AccountManagement.Controllers
{
    [ApiController]
    [Route("api/internal")]
    public class InternalController : ControllerBase
    {

        private readonly dbCont _database;
        public InternalController(dbCont database)
        {
            _database = database;
        }


        [HttpPost("createlobby")]
        public IActionResult CreateLobby([FromBody] CreateLobbyRequest request)
        {
            Lobby lobby = new Lobby();
            lobby.CreationDate = DateTime.Now;
            lobby.HostPlayerID = request.HostPlayerID;
            lobby.SecPlayerID = request.SecPlayerID;
            lobby.LobbyId = request.LobbyID;

            _database.Add(lobby);
            _database.SaveChanges();

            return Ok();
        }
        [HttpPost("updatelobby")]
        public IActionResult UpdateLobby([FromBody] UpdateLobbyRequest request)
        {
            Lobby lobby = _database.Lobbies.FirstOrDefault(p => p.LobbyId == request.LobbyID);
            if (lobby == null)
            {
                return BadRequest();
            }
            lobby.WinnerPlayerID = request.WinnerPlayerID;
            _database.SaveChanges();
            return Ok();
        }
        [HttpPost("updateplayer")]
        public IActionResult UpdatePlayer([FromBody] UpdateWinsRequest request)
        {
            Player player = _database.Players.FirstOrDefault(p => p.PlayerId == request.PlayerID);
            if (player == null)
            {
                return BadRequest();
            }
            player.Wins += request.Wins;
            player.Losses += request.Losses;
            player.Draws += request.Draws;

            return Ok();
        }
    }
}