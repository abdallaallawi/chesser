namespace AccountManagement.DTOs
{
    public class CreateLobbyRequest
    {
        public int LobbyID { get; set; }
        public int HostPlayerID { get; set; }
        public int SecPlayerID { get; set; }
    }
}