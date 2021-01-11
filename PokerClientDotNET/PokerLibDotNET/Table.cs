using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public class Pot
    {
        public float _currRaise;
        public float _amnt;
        public Player _raiser;
        public List<Player> _playerList;
        bool _final;
        public Pot()
        {
            _playerList = new List<Player>();
        }
    }

    public class Seat
    {
        public bool _bOccupied;
        public Player player;
    }
    
    public class Table
    {
        public List<Seat> _Seats;
        public short _DealerIndex;
        public short _CurrentBettor;
        public short _nRoundPlayerCnt;
        public List<Pot> _PotList;
        public TableState _State;

        public Table()
        {
            _Seats = new List<Seat>();
            _PotList = new List<Pot>();
        }
    }

    public class TableHoldem: Table
    {
        public short _BBIndex;
        public short _SBIndex;

        public TableHoldem() : base() { }
    }
}
