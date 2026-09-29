using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace AccountManagement.Models
{
    public class Lobby
    {
        [DatabaseGenerated(DatabaseGeneratedOption.None)]
        public int LobbyId { get; set; }
        [ForeignKey("Player")]
        public int HostPlayerID { get; set; }
        [ForeignKey("Player")]
        public int SecPlayerID { get; set; }
        public DateTime CreationDate { get; set; }
        [ForeignKey("Player")]
        public int WinnerPlayerID { get; set; }
    }
}
