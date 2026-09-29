using System;
using System.Collections.Generic;
using AccountManagement.Models;
using Microsoft.AspNetCore.Mvc.ModelBinding;
using Microsoft.EntityFrameworkCore;

namespace AccountManagement.Data
{
    public class dbCont : DbContext
    {
        public dbCont(DbContextOptions<dbCont> options) : base(options) { }
        public DbSet<Player> Players { get; set; }
        public DbSet<Lobby> Lobbies { get; set; }
    }
}
