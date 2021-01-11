using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PokerLibDotNET;

namespace PokerClientDotNET
{
    public class CardsHelper
    {
        public static bool IsCardInList(List<Card> list, Card card)
        {
            foreach (Card c in list)
            {
                if (c.suite == card.suite &&
                    c.face == card.face) 
                    return true;
            }

            return false;
        }

        private static bool IsCardFaceInList(List<Card> list, Card card)
        {
            foreach (Card c in list)
            {
                if (c.face == card.face) 
                    return true;
            }

            return false;
        }


        private static bool IsCardSuiteInList(List<Card> list, Card card)
        {
            foreach (Card c in list)
            {
                if (c.suite == card.suite) 
                    return true;
            }

            return false;
        }

        public static List<Card> GetWinningCards(NetPacketWinner winner, List<Card> community)
        {
            List<Card> result = new List<Card>();
            foreach (Card c in community)
            {
              if (IsCardFaceInList(winner._Hands, c))
                     result.Add(c);
            }

            //switch (winner._Rank)
            //{
            //    case HandRanking.HighCard: {
            //        foreach (Card c in community) {
            //            if (IsCardInList(winner._Hands, c))
            //                result.Add(c);
            //        }
            //    }break;
            //    case HandRanking.OnePair:
            //    case HandRanking.TwoPair:
            //    case HandRanking.ThreeOfAKind:
            //    case HandRanking.FullHouse:
            //    case HandRanking.FourOfAKind:{
            //        foreach (Card c in community)
            //        {
            //            if (IsCardFaceInList(winner._Hands, c))
            //                result.Add(c);
            //        }
            //    }break;

            //    case HandRanking.Straight: {
            //         int ctr = 0;
            //        FaceCardType face = winner._Hands[0].face - 1;
            //        for(int x = 1; x<community.Count;x++)
            //        {
            //            if(community[x].face == face)
            //                result.Add(community[x]);
            //            face -= ++ctr;
            //        }                    

            //    }break;
            //    case HandRanking.Flush:{
            //        foreach (Card c in community){
            //            if (IsCardSuiteInList(winner._Hands, c))
            //                result.Add(c);       
            //        }
            //        } break;

            //    case HandRanking.StraightFlush:
            //    case HandRanking.RoyalStraighFlush: {
            //        int ctr = 0;
            //        FaceCardType face = winner._Hands[0].face-1;
            //        for(int x = 1; x<community.Count;x++)
            //        {
            //            if(community[x].face == face &&
            //                community[x].suite == winner._Hands[1].suite)
            //                result.Add(community[x]);
            //            face -= ++ctr;
            //        }                    
                    
            //        } break;                
            //}

            return result;
        }
    }
}
