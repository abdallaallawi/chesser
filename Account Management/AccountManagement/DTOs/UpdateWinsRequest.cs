namespace AccountManagement.DTOs
{
    public class UpdateWinsRequest
    {
        public int PlayerID { get; set; }
        public int Wins {  get; set; }
        public int Losses { get; set; }
        public int Draws { get; set; }
    }
}