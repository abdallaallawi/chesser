using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace AccountManagement.Models
{
    public class Player
    {
        [Key]
        public int PlayerId { get; set; }
        public string Username { get; set; }
        public string PasswordHash { get; set; }
        public DateTime CreatedAt { get; set; }
        public int Wins {  get; set; }
        public int Losses { get; set;}
        public int Draws { get; set; }
        public string? RefresherToken { get; set; }
    }
}
