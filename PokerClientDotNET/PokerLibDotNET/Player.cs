using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public class Player
    {
        public List<Card> _HoleCards;
        public int _SessionID;
        public float _fBet;
        public float _TotalChips;
        public PokerAction _Action;
        public short _SeatNum;
        public bool _IsPlaying;

        public Player()
        {
            _HoleCards = new List<Card>();
        }
    }
}
