using System.Text;
using System.Text.Json;
using AccountManagement.Data;
using AccountManagement.DTOs;
using AccountManagement.Models;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.IdentityModel.Tokens;

namespace AccountManagement.Controllers
{

    [ApiController]
    [Route("api/player")]
    [Authorize]
    public class PlayerController : ControllerBase
    {
        private readonly dbCont _database;
        private readonly HttpClient _httpClient;

        public PlayerController(dbCont database, HttpClient httpClient)
        {
            _database = database;
            _httpClient = httpClient;
        }

        [HttpGet("username")]
        public IActionResult GetUsername()
        {
            var ID = User.FindFirst("PlayerID")?.Value;

            if (!int.TryParse(ID, out var PlayerID))
            {
                return BadRequest();
            }

            //var Username = User.Identity?.Name;
            Player player = _database.Players.FirstOrDefault(p => p.PlayerId == PlayerID);

            return Ok(new { Username = player.Username });
        }

        [HttpPost("joinlobby")]
        public async Task<IActionResult> JoinLobby ()
        {
            var ID = User.FindFirst("PlayerID")?.Value;

            if (!int.TryParse(ID, out var PlayerID))
            {
                return BadRequest();
            }

            Player player = _database.Players.FirstOrDefault(p => p.PlayerId == PlayerID);

            if (player == null)
            {
                return NotFound();
            }

            var Username = player.Username;

            string ticket = Guid.NewGuid().ToString("N");

            var message = new
            {
                PlayerID = PlayerID,
                Username = Username,
                Ticket = ticket
            };


            var jsonCont = new StringContent(JsonSerializer.Serialize(message), Encoding.UTF8, "application/json");
            var response = await _httpClient.PostAsync("http://127.0.0.1:8090/internal/issue-ticket", jsonCont);

            if (!response.IsSuccessStatusCode)
            {
                return StatusCode(500, new { Message = "Game server is down." });
            }

            return Ok(new {Ticket = ticket});
        }

    }
}