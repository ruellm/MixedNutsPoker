using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    /* Room specific base */
    public abstract class NetPacketRoom
        : INetPacket
    {
        public GamePacket _gamePacketID;
        public int _nRoomID;
        public NetPacketRoom()
        {
            m_PacketId = PacketIDType.eRoomSpecific;
        }
    }
    
    /* Join Game */
    public class NetPacketJoinGame
        : NetPacketRoom
    {
        public int _SeatNum;
        public float _BuyIn;
        public NetPacketJoinGame()
            : base() 
        {
            _gamePacketID = GamePacket.eJoin;
        }
         public override string ToString()
         {
             //packet_id(space)gamepacketid(space)roomnum;seatnum;|
             string strRawPacket =
                 string.Format("{0}*{1}*{2};{3};{4};|",
                     (int)m_PacketId,
                     (int)_gamePacketID,
                     _nRoomID,
                     _SeatNum,
                     _BuyIn);
             return strRawPacket;
         }
    }
        
    /* Hole Cards*/
    public class NetPacketCards
        : NetPacketRoom
    {
        public List<Card> _Cards;
        public NetPacketCards()
            : base()
        {            
            _Cards = new List<Card>();
        }

        public NetPacketCards(GamePacket id)
            : base()
        {
            _gamePacketID = id;
        }
        public override string ToString()
        {
           //packet_id(space)gamepacketid(space)roomnum;cards1;cards2;..;|
	        string strData ="{0}*{1}*{1};";

            for(int x=0;x<_Cards.Count;x++)
	        {
		        //cards format is "suite/vale;"
                string strCard = string.Format("{0}/{1};",
                    (int)_Cards[x].suite,
                    (int)_Cards[x].face);
        			
		        strData += strCard;
	        }
        	
	        //end data
	        strData += PokerLibConstants.PACKET_DELIMETER;
	        string strRawPacket =
		        string.Format(strData,
			        (int)m_PacketId,
			        (int)_gamePacketID,
			        _nRoomID);

	        return strRawPacket;
        }
    }

   /* Table snap shot - Poker common */
    public class NetPacketPokerTableSnapShot
        : NetPacketRoom
    {
        public Table _Table;
        public NetPacketPokerTableSnapShot()
            : base()
        {
            _gamePacketID = GamePacket.eTableSnapShot;
            _Table = new Table();
        }
        public override string ToString()
        {
            //NOTE: Blank for now, 
            //only server library copy needs this function
            return null;
        }
    }


    /* Table snap shot - Holdem */
    //public class NetPacketHoldemTableSnapShot
    //    : NetPacketRoom
    //{
    //    public TableHoldem _Table;
    //    public NetPacketHoldemTableSnapShot() 
    //        : base ()
    //    {
    //        _gamePacketID = GamePacket.eHoldemTableSnapShot;
    //        _Table = new TableHoldem();
    //    }
    //    public override string ToString()
    //    {
    //        //NOTE: Blank for now, 
    //        //only server library copy needs this function
    //        return null;
    //    }
    //}
    
    /* Wait action*/
    /*TODO: TBD. strucuture of this NetPacketWaitBet not final*/
     public class NetPacketWaitBet
        : NetPacketRoom
    {       
         public NetPacketWaitBet() 
            : base()
         {
             _gamePacketID = GamePacket.eWaitBet;
         }

         public override string ToString()
        {
            //NOTE: Blank for now, 
            //only server library copy needs this function
            return null;
        }
    }

     /* Net packet action */
     public class NetPacketAction
         : NetPacketRoom
     {
         public PokerAction _Action;
         public float _fAmnt;
         public NetPacketAction()
             : base()
         {
             _gamePacketID = GamePacket.eAction;
         }
         public override string ToString()
         {
             //packet_id(space)gamepacketid(space)roomnum;action;amount;playerid;|
	         string strRawPacket =
		        string.Format("{0}*{1}*{2};{3};{4};|",
                    (int)m_PacketId,
                    (int)_gamePacketID,
                    _nRoomID,
                    (int)_Action,
                    _fAmnt);

	        return strRawPacket;
             
         }
     }
     
    /* Net Packet declaring the winner */
     public class NetPacketWinner 
         : NetPacketRoom
     {
        // public int _PlayerID;
         public short _PotNum;
         public HandRanking _Rank;
         public List<Card> _Hands;
         public List<int> _PList;
         public float _Amount;
         public NetPacketWinner() :
             base()
         {
             _gamePacketID = GamePacket.eWinner;
             _Hands = new List<Card>();
             _PList = new List<int>();
         }

         public override string ToString()
         {
             return null;
         }
     }

     /* Net Packet Show cards */
     public class NetPacketShowHands
         : NetPacketRoom
     {
         public int _PlayerID;
         public List<Card> _Hands;
         public NetPacketShowHands():base()
         {
             _gamePacketID = GamePacket.eShowHands;
             _Hands = new List<Card>();
         }

         public override string ToString()
         {
             return null;
         }
     }

     /* chat message */
     public class NetPacketChatMsg
         : NetPacketRoom
     {
         public string _Message;
         public int _PlayerID;

         public NetPacketChatMsg() :
             base()
         {
             _gamePacketID = GamePacket.eChatMsg;
         }

         public override string ToString()
         {
             string strRawPacket =
                string.Format("{0}*{1}*{2};{3};{4};|",
                    (int)m_PacketId,
                    (int)_gamePacketID,
                    _nRoomID,
                    _PlayerID,
                    _Message);

             return strRawPacket;
         }
     }
     /* open cards for seven card stud */
     public class NetPacketOpenCards
         : NetPacketRoom
     {
         public List<CardOwn> _ownedList;
         public NetPacketOpenCards(GamePacket id)
             : base()
         {
             _ownedList = new List<CardOwn>();
             _gamePacketID = id;
         }
         
         public override string ToString()
         {
             return null;
         }
     }

     /* Custom Packet for 1 parameter(int) in Room */
     public class NetPacketRoomInt
         : NetPacketRoom
     {
         public int _Param;
         public NetPacketRoomInt(GamePacket id)
             : base()
         {
             this._gamePacketID = id;
         }
         public NetPacketRoomInt(GamePacket id, int param)
             : base()
         {
             _Param = param;
             this._gamePacketID = id;
         }

         public override string ToString()
         {
             //packet_id(space)gamepacketid(space)roomnum;param;|
	        string strRawPacket =
		       string.Format("{0}*{1}*{2};{3};|",
                    (int)m_PacketId,
                    (int)_gamePacketID,
                    _nRoomID,
                    _Param);

	        return strRawPacket;
         }
     }

}
