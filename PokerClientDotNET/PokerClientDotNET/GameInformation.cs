using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using PokerLibDotNET;
namespace PokerClientDotNET
{
    public class GameInformation
    {
        public GameInformation()
        {
            //_Info = new GameInfoPoker();
        }

        public int _NumPlayers
        { get; set; }

        public GameInfoPoker _Info
        {   get; set; }

        public Table _Table
        {   get; set; }

        public int _GameID
        { get; set; }

        public string Description
        {
            get {
                return _Info.strDescription;
            }
        }
        public string Stake {
            get {          
                return string.Format("${0}/${1}", _Info.stake / 2.0, _Info.stake);
            }        
        }
        public string Players {
            get {
                return string.Format("{0}/{1}", _NumPlayers, _Info.max_players);
            }
        }
        public string Limit
        {
            get
            {
                return string.Format("{0}", _Info.limit_type.ToString().Substring(1,
                    _Info.limit_type.ToString().Length-1));
            }
        }

        public string GameType
        {
            get
            {
                return string.Format("{0}", _Info.game_type.ToString().Substring(1,
                    _Info.game_type.ToString().Length - 1));
            }
        }
    }
}
