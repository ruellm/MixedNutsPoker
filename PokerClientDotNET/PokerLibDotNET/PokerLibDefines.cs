using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
    public class PokerLibConstants
    {
        public const int MAX_BUFFER_SIZE = 4096;
        public const char PACKET_DELIMETER = '|';
        public const char PACKET_PARAMETER_DELIMETER = ';';
        public const char PACKET_ARGUMENT_DELIMETER = '*';
        public const char PACKET_INTERNAL_DELIMETER = '/';
        public const char PACKET_INTERNAL_DELIMTER_2 = '&';

        public const int GAMEDESCMAX = 30;
        public const int POKER_INVALID_POSITION = -1;
        public const string DELIMETER_REPLACEMENT = "[$pipe$]";
        public const string PARAM_DELEM_REPLACEMENT = "[$semicolon$]";
        public const string ARG_DELEM_REPLACEMENT = "[$asterisk$]";
        public const string INTERN_DELEM_REPLACEMENT = "[$slash$]";
        public const string INTERN_DELEM_2_REPLACEMENT = "[$amp$]";

        public const int CLIENT_ID_INVALID = 0;        
        public const int MAX_RAISER_CNT = 4;
     }

    public enum PacketIDType : int
    {
        //Lobby Packets
        eReqGameList = 1,
        eGetClientInfo = 2,
        eGetGameInfo = 3,
        eReqLogIn = 4,
        eEnterRoom = 5,
        eLeaveRoom = 6,
        eCreateGame = 7,
        eRoomSpecific = 8,		//game room specific action
        eGameInfo = 9,		//individual game information
        eGameSpectate = 10,
        eCreateTournament = 11,		//Create tournamet, TODO: admin only
        eGameListItem = 12,
        eReqPokerGameList = 13,
        eCreatePokerGame = 14,
        eStandUp = 15,
        eRegLogInFeedback = 16,
        eClientInfoFeedBack = 17,
        eReqGamePlayerList = 18,
        ePlayerList = 19
    }

    //Room Specific packet
    public enum GamePacket : int
    {
        eJoin = 1,
        eStandup = 2,
        eAction = 3,				//check, fold, raise
        eExit = 4,
        eHoleCard = 5,
        eTableSnapShot = 6,
        eWaitBet = 7,
        
        //holdem packets
        eFlop = 8,
        eTurn = 9,
        eRiver = 10,
        //eHoldemTableSnapShot = 11,
        eWinner = 12,
        eShowHands = 13,            //show hand packet is different depending on direction(serv->client or client->server)
        eChatMsg = 14,
        eCommunityCard = 15,
               
        //7 card stud packets
        eOpenCards = 16,             //for 7 card stud
        e3rdStreet = 22,
        e4thStreetParam = 17,       // used for seven card stud 4th street betting
        e4rthStreet = 18,
        e5thStreet = 19,
        e6thStreet = 20,
        e7thStreet = 21,

        //for SNG and tournament games
        eBlindRaise  = 23
    }

    //Game Type
    public enum GameType : int
    {
        eTexasHoldem = 1,
        eSevenCardStud = 2,
        eOmahaHoldem = 3,
        eSevenCardStudHiLo = 4,
        eRazz = 5,
        eOmahaHiLo = 6
    }

    //Pot limit type
    public enum GameLimitType : int
    {
        eNoLimit = 1,
        ePotLimit = 2,
        eFixedLimit = 3
    }

    //Game mode type
    public enum GameModeType : int
    {
        eModeRingGame = 1, // Cash game
        eModeFreezeOut = 2, // Tournament
        eModeSNG = 3,  // Sit'n'Go
    }

    //Player Action - Poker
    public enum PokerAction : int
    {
        eFold = 1,
        eCheck = 2,
        eRaise = 3,
        eCall = 4,
        eIdle = 0,
        eAllIn = 5,
        eAntes = 6,
        eBringIn = 7
    }

    public enum TableState:int
    {
	    eWaitingPlayers = 0,
	    ePlaying = 1,
        eNewRound = 2,
        //texas/omaha holdem states
        eHoleState = 3,
        eFlopState = 4,
        eTurnState = 5,
        eRiverState = 6,
        //seven card stud
        e4thStreetState = 8,
        e5thStreetState = 9,
        e6thStreetState = 10,
        e7thStreetState = 11,

        //show down state
        eShowDownState = 12,
    }

    public enum StatusType : int
    {
        Success = 1,
        Error = 0
    }

    //helper class
    public class PokerLibGlobal
    {
        public static bool IsPokerGame(GameType type)
        {
            return (type == GameType.eTexasHoldem || 
                type == GameType.eOmahaHoldem || 
                type == GameType.eSevenCardStud||
                type == GameType.eSevenCardStudHiLo ||
                type == GameType.eRazz ||
                type == GameType.eOmahaHiLo);
        }
        public static bool IsSevenCardStudGame(GameType type)
        {
            return (type == GameType.eSevenCardStud ||
                type == GameType.eSevenCardStudHiLo ||
                type == GameType.eRazz);
        }

        public static bool IsOmahaGame(GameType type)
        {
            return (type == GameType.eOmahaHoldem ||
                  type == GameType.eOmahaHiLo);
        }

        public static bool IsHoldemGame(GameType type)
        {
            return (IsOmahaGame(type) ||
                  type == GameType.eTexasHoldem);
        }

        public static bool IsHiLoGame(GameType type)
        {
            return (type == GameType.eSevenCardStudHiLo ||
                  type == GameType.eOmahaHiLo);
        }

        public static bool IsLowBall(GameType type)
        {
            return (type == GameType.eRazz);
        }
    }
}
