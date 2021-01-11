using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public class NetPacketFactory
    {
        public INetPacket BuildFromTokens(string[] tokens)
        {
            INetPacket packet = null;
            PacketIDType id = (PacketIDType)System.Convert.ToInt32(tokens[0], 10);
            switch (id)
            {
                case PacketIDType.eGameListItem:
                    packet = BuildGameListItem(tokens);
                    break;
                case PacketIDType.eRoomSpecific:
                    packet = CreateRoomSpecific(tokens);
                    break;
                case PacketIDType.eRegLogInFeedback: {
                    packet = new NetPacketInt(PacketIDType.eRegLogInFeedback);
                    string[] token_param = tokens[1].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
                    ((NetPacketInt)packet)._nParam = System.Convert.ToInt32(token_param[0], 10);
                    }break;
                case PacketIDType.eClientInfoFeedBack:
                    packet = CreateClientInfo(tokens);
                    break;
                case PacketIDType.ePlayerList:
                    packet = CreatePlayerList(tokens);
                    break;
                    
            }
            return packet;
        }

        private INetPacket CreatePlayerList(string[] tokens)
        {
            NetPacketPlayerList packet = new NetPacketPlayerList();
            string[] token_param = tokens[1].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            packet._roomID = System.Convert.ToInt32(token_param[0], 10);            

            const int headCount = 1;
            int count = token_param.Length - headCount;
            for (int x = 0; x < count - 1; x++)
            {
                string[] playerInfo = token_param[headCount + x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
                Player p = new Player();
                p._SessionID = System.Convert.ToInt32(playerInfo[0], 10);
                p._TotalChips = System.Convert.ToSingle(playerInfo[1]);
                packet._playerList.Add(p);
            }
            return packet;
        }
        
        private INetPacket CreateClientInfo(string[] tokens)
        {
            NetPacketClientInfoFeedBack packet = new NetPacketClientInfoFeedBack();
            string[] token_param = tokens[1].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            packet._PlayerID = System.Convert.ToInt32(token_param[0], 10);
            packet._UserName = token_param[1];
            packet._Location = token_param[2];

            return packet;
        }

        private INetPacket CreateRoomSpecific(string[] tokens)
        {
            INetPacket packet=null;
            GamePacket gpacket = (GamePacket)System.Convert.ToInt32(tokens[1], 10);
            switch(gpacket){
                //case GamePacket.eHoldemTableSnapShot:
                //    packet = CreateTableHoldem(tokens);
                //    break;
                case GamePacket.eTableSnapShot:
                    packet = CreatePokerTable(tokens);
                    break;
                case GamePacket.eWaitBet:{
                    packet = new NetPacketWaitBet();
                    string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
                    ((NetPacketRoom)packet)._nRoomID = System.Convert.ToInt32(token_param[0],10);
                    }break;
                case GamePacket.eHoleCard:
                case GamePacket.eFlop:
                case GamePacket.eTurn:    
                case GamePacket.eRiver:
                case GamePacket.eCommunityCard:
                case GamePacket.e7thStreet:
                    packet = CreateCards(tokens, gpacket);
                    break;
                case GamePacket.eWinner:
                    packet = CreateWinner(tokens);
                    break;
                case GamePacket.eShowHands:
                    packet = CreateShowHand(tokens);
                    break;            
                case GamePacket.eChatMsg:
                    packet = CreateChatMsg(tokens);
                    break;
                case GamePacket.eOpenCards:
                case GamePacket.e3rdStreet:
                case GamePacket.e4rthStreet:
                case GamePacket.e5thStreet:
                case GamePacket.e6thStreet:
                    packet = CreateOpenCards(tokens, gpacket);
                    break;
                case GamePacket.eBlindRaise:
                case GamePacket.e4thStreetParam: {
                    packet = new NetPacketRoomInt(gpacket);
                    string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
                    ((NetPacketRoom)packet)._nRoomID = System.Convert.ToInt32(token_param[0], 10);
                    ((NetPacketRoomInt)packet)._Param = System.Convert.ToInt32(token_param[1], 10);
                    }break;

            }
            return packet;
        }
        private INetPacket CreatePokerTable(string[] tokens)
        {
            NetPacketPokerTableSnapShot packet = new NetPacketPokerTableSnapShot();
            string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            const int tableHeaderCnt = 6;

            //packet_id(space)gamepacketid(space)roomnum;State;current_bettor;curr_raise;pots_info;player1;...;playern;|
            //player info: player_id/totalchips/bet;
            //pot info is: pot0/pot1/../potn;

            //populate table values
            packet._nRoomID = System.Convert.ToInt32(token_param[0], 10);
            packet._Table._State = (TableState)System.Convert.ToInt32(token_param[1], 10);
            packet._Table._DealerIndex = System.Convert.ToInt16(token_param[2], 10);
            packet._Table._CurrentBettor = System.Convert.ToInt16(token_param[3], 10);

            //(Re)Build Pot Information
            string[] token_potInfo = token_param[5].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
            packet._Table._PotList.Clear();
            for (int idx = 0; idx < token_potInfo.Length - 1; idx++)
            {
                Pot pot = new Pot();
                pot._currRaise = 0;
                pot._amnt = System.Convert.ToSingle(token_potInfo[idx]);
                packet._Table._PotList.Add(pot);
            }
            //set current raise value
            Pot currPot = packet._Table._PotList[packet._Table._PotList.Count - 1];
            currPot._currRaise = System.Convert.ToSingle(token_param[4]);

            //count number of chairs for this table
            int count = token_param.Length - tableHeaderCnt;
            for (int x = 0; x < count - 1; x++)
            {
                string[] tokenPlayer = token_param[tableHeaderCnt + x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
                Seat s = new Seat();
                Player p = new Player();
                p._SessionID = System.Convert.ToInt32(tokenPlayer[0], 10);
                p._TotalChips = System.Convert.ToSingle(tokenPlayer[1]);
                p._fBet = System.Convert.ToSingle(tokenPlayer[2]);
                p._Action = (PokerAction)System.Convert.ToInt32(tokenPlayer[3], 10);
                p._IsPlaying = Convert.ToBoolean(System.Convert.ToInt32(tokenPlayer[4], 10));
                //p._SeatNum = (short)x;
                s._bOccupied = (p._SessionID == 0) ? false : true;
                s.player = p;
                packet._Table._Seats.Add(s);
            }
            return packet;
        }

        private INetPacket CreateOpenCards(string[] tokens,GamePacket id)
        {
            NetPacketOpenCards packet = new NetPacketOpenCards(id);
            string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            ((NetPacketRoom)packet)._nRoomID = System.Convert.ToInt32(token_param[0], 10);
            const int head = 1;

            for (int x = 0; x < token_param.Length - head - 1; x++)
            {
                string[] token_cardowner= token_param[head + x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMTER_2);
                CardOwn own = new CardOwn();
                own.playerID = System.Convert.ToInt32(token_cardowner[0], 10);
                for (int c = 0; c<token_cardowner.Length - head; c++) {
                    string[] token_cards = token_cardowner[head + c].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
                    Card card = new Card();

                    card.suite = (SuiteType)System.Convert.ToInt32(token_cards[0], 10);
                    card.face = (FaceCardType)System.Convert.ToInt32(token_cards[1], 10);
                    own.hands.Add(card);
                }
                packet._ownedList.Add(own);
            }            

            return packet;
        }

        private INetPacket CreateChatMsg(string[] tokens)
        {
            NetPacketChatMsg packet = new NetPacketChatMsg();
            string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);

            ((NetPacketRoom)packet)._nRoomID = System.Convert.ToInt32(token_param[0], 10);
            packet._PlayerID = System.Convert.ToInt32(token_param[1], 10);
            packet._Message = token_param[2];

            return packet;
        }

        private INetPacket CreateShowHand(string[] tokens)
        {
            NetPacketShowHands packet = new NetPacketShowHands();
            string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);

            ((NetPacketRoom)packet)._nRoomID = System.Convert.ToInt32(token_param[0], 10);
            packet._PlayerID = System.Convert.ToInt32(token_param[1], 10);
            
            const int head = 2;
            for (int x = 0; x < token_param.Length - head - 1; x++)
            {
                Card card = new Card();
                string[] token_cards = token_param[head + x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);

                card.suite = (SuiteType)System.Convert.ToInt32(token_cards[0], 10);
                card.face = (FaceCardType)System.Convert.ToInt32(token_cards[1], 10);
                packet._Hands.Add(card);
            }

            return packet;
        }

        private INetPacket CreateWinner(string[] tokens)
        {
            NetPacketWinner packet = new NetPacketWinner();
            string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);

            ((NetPacketRoom)packet)._nRoomID = System.Convert.ToInt32(token_param[0], 10);
            packet._PotNum = System.Convert.ToInt16(token_param[1], 10);
            packet._Rank = (HandRanking)System.Convert.ToInt32(token_param[2], 10);
            packet._Amount = System.Convert.ToSingle(token_param[3]);
            
            string[] token_pID = token_param[4].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
            
            if (!token_param[4].Equals(""))
            {
                for (int x = 0; x < token_pID.Length; x++)
                {
                    int pID = System.Convert.ToInt32(token_pID[x], 10);
                    packet._PList.Add(pID);
                }
            }                        
            const int head1 = 5;
            if (!token_param[head1].Equals(""))
            {
                for (int x = 0; x < token_param.Length - head1 - 1; x++)
                {
                    Card card = new Card();
                    string[] token_cards = token_param[head1 + x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);

                    card.suite = (SuiteType)System.Convert.ToInt32(token_cards[0], 10);
                    card.face = (FaceCardType)System.Convert.ToInt32(token_cards[1], 10);
                    packet._Hands.Add(card);
                }
            }

            return packet;
        }

        private INetPacket CreateCards(string[] tokens,GamePacket id)
        {
            NetPacketCards packet = new NetPacketCards();
            packet._gamePacketID = id;
            string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            const int head = 1;

            packet._nRoomID = System.Convert.ToInt32(token_param[0], 10);
            for (int x = 0; x < token_param.Length - head - 1; x++) {
                Card card = new Card();
                string[] token_cards = token_param[head+x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);

                card.suite = (SuiteType)System.Convert.ToInt32(token_cards[0], 10);
                card.face = (FaceCardType)System.Convert.ToInt32(token_cards[1], 10);
                packet._Cards.Add(card);
            }

            return packet;
        }

        //private INetPacket CreateTableHoldem(string[] tokens)
        //{            
        //    NetPacketHoldemTableSnapShot packet = new NetPacketHoldemTableSnapShot();
        //    string[] token_param = tokens[2].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
        //    const int tableHeaderCnt = 8;

        //    //packet_id(space)gamepacketid(space)roomnum;State;dealerIdx;SB;BB;current_bettor;curr_raise;pots_info;player1;...;playern;|
        //    //player info: player_id/totalchips/bet;
        //    //pot info is: pot0/pot1/../potn;

        //    //populate table values
        //    packet._nRoomID = System.Convert.ToInt32(token_param[0], 10);
        //    packet._Table._State = (TableState)System.Convert.ToInt32(token_param[1], 10);
        //    packet._Table._DealerIndex = System.Convert.ToInt16(token_param[2], 10);
        //    packet._Table._SBIndex = System.Convert.ToInt16(token_param[3], 10);
        //    packet._Table._BBIndex = System.Convert.ToInt16(token_param[4], 10);
        //    packet._Table._CurrentBettor = System.Convert.ToInt16(token_param[5], 10);
            
        //    //(Re)Build Pot Information
        //    string[] token_potInfo = token_param[7].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
        //    packet._Table._PotList.Clear();
        //    for (int idx = 0; idx < token_potInfo.Length - 1; idx++) {
        //        Pot pot = new Pot();
        //        pot._currRaise = 0;
        //        pot._amnt = System.Convert.ToSingle(token_potInfo[idx]);
        //        packet._Table._PotList.Add(pot);
        //    }
        //    //set current raise value
        //    Pot currPot = packet._Table._PotList[packet._Table._PotList.Count - 1];
        //    currPot._currRaise = System.Convert.ToSingle(token_param[6]);

        //    //count number of chairs for this table
        //    int count = token_param.Length - tableHeaderCnt;
        //    for (int x = 0; x < count-1; x++) {
        //        string[] tokenPlayer = token_param[tableHeaderCnt + x].Split(PokerLibConstants.PACKET_INTERNAL_DELIMETER);
        //        Seat s = new Seat();
        //        Player p = new Player();
        //        p._SessionID = System.Convert.ToInt32(tokenPlayer[0], 10);
        //        p._TotalChips = System.Convert.ToSingle(tokenPlayer[1]);
        //        p._fBet = System.Convert.ToSingle(tokenPlayer[2]);
        //        p._Action =(PokerAction) System.Convert.ToInt32(tokenPlayer[3], 10);                
        //        p._IsPlaying = Convert.ToBoolean(System.Convert.ToInt32(tokenPlayer[4], 10));
        //        //p._SeatNum = (short)x;
        //        s._bOccupied = (p._SessionID==0)?false:true;
        //        s.player = p;
        //        packet._Table._Seats.Add(s);
        //    }

        //    return packet;
        //}

        private INetPacket BuildGameListItem(string[] tokens)
        {
            string[] token_param = tokens[1].Split(PokerLibConstants.PACKET_PARAMETER_DELIMETER);
            GameType type = (GameType)System.Convert.ToInt32(token_param[1], 10);
            NetPacketGameListItem item = null;

            if (PokerLibGlobal.IsPokerGame(type))
            {
                //if (type == GameType.eSevenCardStud)
                //{
                //    item = new NetPacketSevenCardStudGameListItem();
                //    ((NetPacketSevenCardStudGameListItem)item)._fAntes = System.Convert.ToSingle(token_param[7]);
                //    ((NetPacketSevenCardStudGameListItem)item)._fBringIn = System.Convert.ToSingle(token_param[8]);
                //}
                //else
                //    item = new NetPacketPokerGameListItem();
                item = new NetPacketPokerGameListItem();
                ((NetPacketPokerGameListItem)item)._gameLimit = (GameLimitType)System.Convert.ToInt32(token_param[6], 10);
                ((NetPacketPokerGameListItem)item)._fAntes = System.Convert.ToSingle(token_param[7]);
                ((NetPacketPokerGameListItem)item)._fBringIn = System.Convert.ToSingle(token_param[8]);   
                ((NetPacketPokerGameListItem)item)._RaiseTime = System.Convert.ToInt32(token_param[9], 10);
                ((NetPacketPokerGameListItem)item)._RaiseFactor = System.Convert.ToSingle(token_param[10]);             
            }
            else {
                item = new NetPacketGameListItem();
            }
            item._type = type;
            item._gameID = System.Convert.ToInt32(token_param[0], 10);
            item._strGameDesc = token_param[3];
            item._fStake = System.Convert.ToSingle(token_param[2]);
            item._nPlayers = System.Convert.ToInt32(token_param[4], 10);
            item._nMaxPlayers = System.Convert.ToInt32(token_param[5], 10);
            return item;
        }
    }
}
