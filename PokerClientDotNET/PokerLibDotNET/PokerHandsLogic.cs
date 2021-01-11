using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public enum HandRanking : int
    {
		HighCard=0,
		OnePair,
		TwoPair,
		ThreeOfAKind,
		Straight,
		Flush,
		FullHouse,
		FourOfAKind,
		StraightFlush,
		RoyalStraighFlush,
		NoRank = 100,
        LowHand = 101
	};

    public class PokerHandEvaluator
    {
        static string[] RankingNames = new string[]{"High card",
                                        "Pair",
                                        "Two Pair",
                                		"Three Of a kind",
		                                "Straight",
		                                "Flush",
		                                "Full house",
		                                "Four of a kind",
		                                "Straight flush",
		                                "Royal straigh flush"};
        
         public static string GetHandString(HandRanking ranking, List<Card> hand) 
        {
            string strResult="";
            
            switch (ranking) { 
                case HandRanking.HighCard:
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString();
                    strResult += " with " + hand[1].face.ToString() + " Kicker";
                    break;
                case HandRanking.OnePair:
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString();
                    strResult += " with " + hand[2].face.ToString() + " Kicker";
                    break;
                case HandRanking.TwoPair:
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString();
                    strResult += " and " + hand[2].face.ToString();
                    strResult += " with " + hand[4].face.ToString() + " Kicker";
                    break;
                case HandRanking.ThreeOfAKind:                    
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString();
                    strResult += " with " + hand[3].face.ToString() + " Kicker";
                    break;
                case HandRanking.Straight:
                    strResult += RankingNames[(int)ranking] + " To " + hand[0].face.ToString();
                    break;
                case HandRanking.Flush:
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString() + " high";
                    break;
                case HandRanking.FullHouse:
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString() + " full of ";
                    strResult += hand[3].face.ToString() + "s";
                    break;
                case HandRanking.FourOfAKind:
                    strResult += RankingNames[(int)ranking] + " " + hand[0].face.ToString();
                    strResult += " with " + hand[4].face.ToString() + " Kicker";
                    break;
                case HandRanking.StraightFlush:
                    strResult += RankingNames[(int)ranking] + " To " + hand[0].face.ToString();
                    break;
                case HandRanking.RoyalStraighFlush:
                    strResult += RankingNames[(int)ranking] + "!";
                    break;

            }

            return strResult;
        
        }

    }
}
