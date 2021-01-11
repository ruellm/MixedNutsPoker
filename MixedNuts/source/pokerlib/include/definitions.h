//TODO: this header file might/can be divided in the future, if contents are saturated
#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#define MAX_BUFFER_SIZE 4096
#define POKER_INVALID_POSITION (-1)
#define PACKET_DELIMETER "|"
#define PACKET_PARAMETER_DELIMETER ";"
#define PACKET_ARGUMENT_DELIMETER  "*"
#define PACKET_INTERNAL_DELIMETER "/"
#define PACKET_INTERNAL_DELIMTER_2 "&"

#define DELIMETER_REPLACEMENT		"[$pipe$]"
#define PARAM_DELEM_REPLACEMENT		"[$semicolon$]"
#define ARG_DELEM_REPLACEMENT		"[$asterisk$]"
#define INTERN_DELEM_REPLACEMENT	"[$slash$]"
#define INTERN_DELEM_2_REPLACEMENT	"[$amp$]"

//Start of user session ID count, 
//userID < 100 means special account											
//userID 0 means invalid or none
//TODO: TBD with userID from DB
#define CLIENT_SESSION_ID_BASE	100
#define CLIENT_ID_INVALID 0

//definitions
typedef unsigned SessionIDType;
typedef unsigned GameIDType;
typedef unsigned IDType;

//packet types definitions
typedef enum  {
	//Lobby Packets
	eReqGameList	= 1,		
	eGetClientInfo	= 2,
	eGetGameInfo	= 3,
	eReqLogIn		= 4,
	eEnterRoom		= 5,
	eLeaveRoom		= 6,
	eCreateGame		= 7,		
	eRoomSpecific	= 8,		//game room specific action
	eGameInfo		= 9,		//individual game information
	eGameSpectate	= 10,	
	eCreateTournament = 11,		//Create tournamet, TODO: admin only
	eGameListItem   = 12,
	eReqPokerGameList = 13,	
	eCreatePokerGame = 14,
	eStandUp = 15,
	eRegLogInFeedback = 16,
	eClientInfoFeedBack = 17,
	eReqGamePlayerList = 18,
	ePlayerList = 19
}PacketIDType;

//Room Specific packet
//TODO: note eFlop,eRiver,eTurn and e3rdStreet..7thSteet are unused in client side
// need to refactor to clean these definitions
typedef enum {
	eJoin	 = 1,
	eStandup = 2,
	eAction  = 3,				//check, fold, raise
	eExit	 = 4,
	eHoleCard = 5,
	eTableSnapShot = 6,
	eWaitBet = 7,		
	
	//holdem packets
	eFlop = 8,
	eTurn = 9,
	eRiver = 10,
	//eHoldemTableSnapShot = 11,
	eWinner = 12,
	eShowHands = 13,				//show hand packet is different depending on direction(serv->client or client->server)
	eChatMsg = 14,
	eCommunityCard = 15,
	 
	//7 card stud packets
	eOpenCards = 16,             //for 7 card stud, during join sessions
	e3rdStreet = 22,
	e4thStreetParam = 17,		// used for seven card stud 4th street betting
	e4rthStreet = 18,
	e5thStreet = 19,
	e6thStreet = 20,
	e7thStreet = 21,

	//for SNG and tournament games
	eBlindRaise  = 23,	
}GamePacket;

//Game Type
typedef enum {
	eTexasHoldem = 1,
	eSevenCardStud = 2,
	eOmahaHoldem = 3,
	eSevenCardStudHiLo=4,
	eRazz = 5,
	eOmahaHiLo = 6
}GameType;

//Pot limit type
typedef enum{
	eNoLimit	= 1,
	ePotLimit	= 2,
	eFixedLimit	= 3,
}GameLimitType;

//Game mode type
typedef enum {
	eModeRingGame	= 1, // Cash game
	eModeFreezeOut	= 2, // Tournament
	eModeSNG		= 3,  // Sit'n'Go
} GameModeType;

//Player Action - Poker
typedef enum{
	eFold = 1,
	eCheck = 2,
	eRaise = 3,
	eCall = 4,
	eIdle = 0,
	eAllIn = 5,
	eAntes = 6,
	eBringIn = 7
}PokerAction;

typedef enum {
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
}TableState;

typedef enum {
	eSuccess = 1,
	eError = 0,
}StatusType;


//common includes
#include "GameInfo.h"

//Macros
#define ISPOKERGAME(x) (x==eTexasHoldem || x==eSevenCardStud || \
						x==eOmahaHoldem || x==eSevenCardStudHiLo || \
						x==eRazz ||	x==eOmahaHiLo)

//common function pointer
typedef void (*FNPOINTER)();

#endif