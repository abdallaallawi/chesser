using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AccountManagement.Migrations
{
    /// <inheritdoc />
    public partial class InitialBuild : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "Lobbies",
                columns: table => new
                {
                    LobbyId = table.Column<int>(type: "int", nullable: false),
                    HostPlayerID = table.Column<int>(type: "int", nullable: false),
                    SecPlayerID = table.Column<int>(type: "int", nullable: false),
                    CreationDate = table.Column<DateTime>(type: "datetime2", nullable: false),
                    WinnerPlayerID = table.Column<int>(type: "int", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Lobbies", x => x.LobbyId);
                });

            migrationBuilder.CreateTable(
                name: "Players",
                columns: table => new
                {
                    PlayerId = table.Column<int>(type: "int", nullable: false)
                        .Annotation("SqlServer:Identity", "1, 1"),
                    Username = table.Column<string>(type: "nvarchar(max)", nullable: false),
                    PasswordHash = table.Column<string>(type: "nvarchar(max)", nullable: false),
                    CreatedAt = table.Column<DateTime>(type: "datetime2", nullable: false),
                    Wins = table.Column<int>(type: "int", nullable: false),
                    Losses = table.Column<int>(type: "int", nullable: false),
                    Draws = table.Column<int>(type: "int", nullable: false),
                    RefresherToken = table.Column<string>(type: "nvarchar(max)", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_Players", x => x.PlayerId);
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "Lobbies");

            migrationBuilder.DropTable(
                name: "Players");
        }
    }
}
