using System.ComponentModel;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Security.Cryptography;
using System.Text;
using AccountManagement.Data;
using AccountManagement.DTOs;
using AccountManagement.Models;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.IdentityModel.Tokens;


namespace AccountManagement.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class AuthController : ControllerBase
    {

        private readonly dbCont _database;              //Access to database

        private readonly IConfiguration _iconfig;       //Access to the appsettings.json

        public AuthController(dbCont database, IConfiguration iconfig)
        {
            _database = database;
            _iconfig = iconfig;
        }


        //Generate a per-login token to identify user whenever they ask for backend resource
        private string GenerateAccessToken(Player player, string secretKey) 
        {
            var claims = new[]
            {
                new Claim(JwtRegisteredClaimNames.Sub, player.Username),
                new Claim("PlayerID", player.PlayerId.ToString()),
                new Claim(JwtRegisteredClaimNames.Jti, Guid.NewGuid().ToString()),
            };
            var key = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(secretKey));
            var credentials = new SigningCredentials(key, SecurityAlgorithms.HmacSha256);

            var tokenDescriptor = new SecurityTokenDescriptor
            {
                Subject = new ClaimsIdentity(claims),
                Expires = DateTime.UtcNow.AddMinutes(15), // Set 15-minute expiration
                SigningCredentials = credentials,
                Issuer = "ChesserAPI",
                Audience = "ChesserPlayers"
            };


            var tokenHandler = new JwtSecurityTokenHandler();
            var token = tokenHandler.CreateToken(tokenDescriptor);

            return tokenHandler.WriteToken(token);
        }

        private string GenerateRefreshToken()
        {
            var randomNumber = new byte[32];

            using var rng = RandomNumberGenerator.Create();
            rng.GetBytes(randomNumber);


            return Convert.ToBase64String(randomNumber);
        }

        [HttpPost("login")]
        public IActionResult Login ([FromBody] LoginRequest request)
        {

            //Authenticate player directly from the database
            Player foundPlayer = _database.Players.FirstOrDefault(u => u.Username == request.Username);

            if (foundPlayer == null)
            {
                return Unauthorized(new { message = "Incorrect username or password." });
            }

            //Hash the received password, then compare it to the hashed password related to the username stored in the database
            bool isPasswordCorrect = BCrypt.Net.BCrypt.Verify(request.Password, foundPlayer.PasswordHash);

            if (!isPasswordCorrect)
            {
                return Unauthorized(new { message = "Incorrect username or password." });
            }

            //Generate per-login token to give to the player when they login


            var secretKey = _iconfig["JwtSettings:SecretKey"];
            string accessToken = GenerateAccessToken(foundPlayer, secretKey);
            string refresherToken = GenerateRefreshToken();
            
            foundPlayer.RefresherToken = refresherToken;
            _database.SaveChanges();

            return Ok(new
            {
                Message = "Successful login",
                AccessToken = accessToken,
                RefresherToken = refresherToken
            });
        }
        [HttpPost("register")]
        public IActionResult Register([FromBody] RegRequest request)
        {
            bool isTaken = _database.Players.Any(u => u.Username == request.Username);
            if (isTaken)
            {
                return Conflict("Username already exists.");
            }
            Player player = new Player();
            player.Username = request.Username;
            player.PasswordHash = BCrypt.Net.BCrypt.HashPassword(request.Password);
            player.Wins = 0;
            player.Losses = 0;
            player.CreatedAt = DateTime.Now;
            player.Draws = 0;

            _database.Players.Add(player);
            _database.SaveChanges();
            return Ok(new {message = "Account registered successfully!"});
        }

        [HttpPost("refresh")]
        public IActionResult RefreshToken([FromBody] RefreshTokenRequest request)
        {
            if (string.IsNullOrWhiteSpace(request.RefreshToken))
            {
                return BadRequest(new { message = "Refresh token is required." });
            }

            var foundPlayer = _database.Players.FirstOrDefault(p => p.RefresherToken == request.RefreshToken);

            if (foundPlayer == null)
            {
                return Unauthorized();
            }
            string secretKey = _iconfig["JwtSettings:SecretKey"];
            string accessToken = GenerateAccessToken(foundPlayer, secretKey);

            string newRefToken = GenerateRefreshToken();
            foundPlayer.RefresherToken = newRefToken;
            _database.SaveChanges();

            return Ok(new
            {
                AccessToken = accessToken,
                RefresherToken = newRefToken
            });
        }


        [Authorize]
        [HttpPost("logout")]
        public IActionResult Logout()
        {
            var ID = User.FindFirst("PlayerID")?.Value;
            if (!int.TryParse(ID, out var PlayerID))
            {
                return BadRequest();
            }
            Player player = _database.Players.FirstOrDefault(p => p.PlayerId == PlayerID);
            if (player != null)
            {
                player.RefresherToken = null;
                _database.SaveChanges();
            }
            return Ok(new { Message = "Sucessfull logout" });
        }
    }
}
