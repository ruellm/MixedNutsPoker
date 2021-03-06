﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public enum SuiteType :int 
    {
        Clubs = 0,
        Diamond = 1,
        Heart = 2,
        Spade = 3,
        None = 100
    }

    public enum FaceCardType : int 
    {
	    Two = 2,
	    Three = 3, 
	    Four = 4, 
	    Five = 5, 
	    Six = 6, 
	    Seven = 7,
	    Eight = 8, 
	    Nine = 9,
	    Ten = 10, 
	    Jack = 11,
	    Queen = 12,
	    King = 13,
        Ace = 14,
	    Joker = 100
    }

    public class Card{
	    public SuiteType suite;
	    public FaceCardType face;
    }

    public class CardOwn
    {
        public int playerID;
        public List<Card> hands;
        public CardOwn()
        {
            hands = new List<Card>();
        }
    }
}
