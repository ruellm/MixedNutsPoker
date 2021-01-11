#include "NetPacketGame.h"
#include "StringHelper.h"

/* Room base */
NetPacketRoom::NetPacketRoom()
{
	m_sPacketID = eRoomSpecific;
}

IDType 
NetPacketRoom::GetRoomID() const
{
	return m_nRoomID;
}
GamePacket 
NetPacketRoom::GetCommand() const
{
	return m_GamePacketID;
}
void 
NetPacketRoom::SetRoom(IDType room)
{
	m_nRoomID=room;
}
std::string
NetPacketRoom::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;|
	std::string strRawPacket =
		format("%d*%d*%d;|",
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID);

	return strRawPacket;
}

/* Join Game */
NetPacketJoinGame::NetPacketJoinGame(void)
	:NetPacketRoom(),m_fBuyIn(0.0),m_nSeatNum(0)
{
	m_GamePacketID = eJoin;
}

std::string
NetPacketJoinGame::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;seatnum;buyin;|
	std::string strRawPacket =
		format("%d*%d*%d;%d;%f;|",
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_nSeatNum,
			m_fBuyIn);

	return strRawPacket;
}


short  
NetPacketJoinGame::GetSeatNum() const
{
	return m_nSeatNum;
}

void 
NetPacketJoinGame::SetSeatNum(short seatno)
{
	m_nSeatNum = seatno;
}
void 
NetPacketJoinGame::SetBuyIn(float amt)
{
	m_fBuyIn = amt;
}
float 
NetPacketJoinGame::GetBuyIn() const
{
	return m_fBuyIn;
}

/* Hole cards */
NetPacketCards::NetPacketCards(GamePacket id,
								IDType room,
								const std::vector<Card>& cards)
	: NetPacketRoom(),m_Cards(cards)
{
	m_GamePacketID = id;
	m_nRoomID=room;
}

std::string 
NetPacketCards::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;cards1;cards2;..;|
	std::string strData ="%d*%d*%d;";

	for(std::vector<Card>::iterator it = m_Cards.begin(); it != m_Cards.end(); it++)
	{
		//cards format is "suite/vale;"
		std::string strCard = format("%d/%d;",
			(*it).GetSuite(),
			(*it).GetFace());
			
		strData += strCard;
	}
	
	//end data
	strData += PACKET_DELIMETER;
	std::string strRawPacket =
		format(strData.c_str(),
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID);

	return strRawPacket;
}

/* Table snap shot - Poker common */
NetPacketPokerTableSnapShot::NetPacketPokerTableSnapShot(boost::shared_ptr<Table> table,IDType room)	
	: m_pTable(table),NetPacketRoom()
{
	m_nRoomID=room;
	m_GamePacketID = eTableSnapShot;
}

std::string 
NetPacketPokerTableSnapShot::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;State;dealeridx;current_bettor;curr_raise;pots_info;player1;...;playern;|
	//player info: player_id/totalchips/bet/action/isplaying;
	//pot info is: pot0/pot1/../potn;
	unsigned idx=0;
	std::string strData ="%d*%d*%d;%d;%d;%d;%f;";
	std::string strPotInfo;
	
	//collect pot snapshot
	for(idx=0;idx<m_pTable->GetPotSize();idx++)
	{
		std::string strPot = format("%f",
			m_pTable->GetPotAt(idx)->amnt);
		
		strPot += PACKET_INTERNAL_DELIMETER;
		strPotInfo += strPot;
	}
	strPotInfo += PACKET_PARAMETER_DELIMETER;
	strData += strPotInfo;
	
	//collect player snapshot
	for(idx=0;idx<m_pTable->GetSeatCount();idx++){
		Player* p = m_pTable->GetPlayerAt(idx);
		std::string strPlayer;
		if( p == NULL ){
			strPlayer = format("%d/%f/%f/%d/%d;",
			0,		//User ID = 0 means none or invalid
			0.0,
			0.0,
			0,
			eIdle);	
		}
		else {
			strPlayer = format("%d/%f/%f/%d/%d;",
			p->GetID(),
			p->GetTotalChips(),
			p->GetBet(),
			p->GetAction(),
			(int)p->IsPlaying());	
		}
		
		strData += strPlayer;
	}
	
	//end data
	strData += PACKET_DELIMETER;
	std::string strRawPacket =
		format(strData.c_str(),
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_pTable->GetState(),
			m_pTable->GetDealer(),
			m_pTable->GetCurrentBettor(),
			m_pTable->GetCurrentPot()->currRaise);

	return strRawPacket;
}


/* Table snap shot - Holdem */
//NetPacketHoldemTableSnapShot::NetPacketHoldemTableSnapShot(boost::shared_ptr<TableHoldem> table,IDType room)	
//	: m_pTable(table),NetPacketRoom()
//{
//	m_nRoomID=room;
//	m_GamePacketID = eHoldemTableSnapShot;
//}

//std::string 
//NetPacketHoldemTableSnapShot::ToString()
//{
//	//packet_id(space)gamepacketid(space)roomnum;State;dealerIdx;SB;BB;current_bettor;curr_raise;pots_info;player1;...;playern;|
//	//player info: player_id/totalchips/bet/action/isplaying;
//	//pot info is: pot0/pot1/../potn;
//	int idx=0;
//	std::string strData ="%d*%d*%d;%d;%d;%d;%d;%d;%f;";
//	std::string strPotInfo;
//	
//	//collect pot snapshot
//	for(idx=0;idx<m_pTable->GetPotSize();idx++)
//	{
//		std::string strPot = format("%f",
//			m_pTable->GetPotAt(idx)->amnt);
//		
//		strPot += PACKET_INTERNAL_DELIMETER;
//		strPotInfo += strPot;
//	}
//	strPotInfo += PACKET_PARAMETER_DELIMETER;
//	strData += strPotInfo;
//	
//	//collect player snapshot
//	for(idx=0;idx<m_pTable->GetSeatCount();idx++){
//		Player* p = m_pTable->GetPlayerAt(idx);
//		std::string strPlayer;
//		if( p == NULL ){
//			strPlayer = format("%d/%f/%f/%d/%d;",
//			0,		//User ID = 0 means none or invalid
//			0.0,
//			0.0,
//			0,
//			eIdle);	
//		}
//		else {
//			strPlayer = format("%d/%f/%f/%d/%d;",
//			p->GetID(),
//			p->GetTotalChips(),
//			p->GetBet(),
//			p->GetAction(),
//			(int)p->IsPlaying());	
//		}
//		
//		strData += strPlayer;
//	}
//	
//	//end data
//	strData += PACKET_DELIMETER;
//	std::string strRawPacket =
//		format(strData.c_str(),
//			m_sPacketID,
//			m_GamePacketID,
//			m_nRoomID,
//			m_pTable->GetState(),
//			m_pTable->GetDealer(),
//			m_pTable->GetSBIndex(),
//			m_pTable->GetBBIndex(),
//			m_pTable->GetCurrentBettor(),
//			m_pTable->GetCurrentPot()->currRaise);
//
//	return strRawPacket;
//}


/* Wait action */
NetPacketWaitBet::NetPacketWaitBet(IDType room):
	NetPacketRoom()
{
	m_nRoomID=room;
	m_GamePacketID = eWaitBet;
}

std::string 
NetPacketWaitBet::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;|
	std::string strRawPacket =
		format("%d*%d*%d;|",
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID);

	return strRawPacket;
}

/* Net packet action */

NetPacketAction::NetPacketAction(IDType room)
	: NetPacketRoom(),m_Action(eIdle),m_fAmount(0.0)/*,m_nSeat(0)*/
{
	m_nRoomID=room;
	m_GamePacketID = eAction;

}

NetPacketAction::NetPacketAction(PokerAction action, float amnt,IDType room/*,short seatnum*/)
	: NetPacketRoom(),m_Action(action),m_fAmount(amnt)/*,m_nSeat(seatnum)*/
{
	m_nRoomID=room;
	m_GamePacketID = eAction;
}

PokerAction 
NetPacketAction::GetAction() const
{ return m_Action; }

float 
NetPacketAction::GetAmount() const
{ return m_fAmount; }

void 
NetPacketAction::SetAction(PokerAction action)
{ m_Action = action; }

void 
NetPacketAction::SetAmount(float amnt)
{ m_fAmount= amnt; }

//short 
//NetPacketAction::GetSeatNum() const
//{	return m_nSeat; }
//
//void 
//NetPacketAction::SetSeatNum(IDType val)
//{	m_nSeat = val; }

std::string 
NetPacketAction::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;action;amount;playerid;|
	std::string strRawPacket =
		format("%d*%d*%d;%d;%f;|",
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_Action,
			m_fAmount/*,
			m_nSeat*/);

	return strRawPacket;
}
/* Net Packet declaring the winner */

NetPacketWinner::NetPacketWinner():
	NetPacketRoom(),m_PotNum(0),
	m_nRank(HandStrength::NoRank),
	m_fAmnt(0.0f)
{
	m_GamePacketID = eWinner;
	m_Hands.clear();
	m_PlayerList.clear();
}

void 
NetPacketWinner::SetPlayerList(std::vector<IDType> players)
{  m_PlayerList=players;}

void 
NetPacketWinner::SetPotNum(short potnum)
{ m_PotNum = potnum; }

void 
NetPacketWinner::SetHandRank(HandStrength::HandRanking rank)
{ m_nRank = rank; }

void 
NetPacketWinner::SetHands(std::vector<Card>& hands)
{ m_Hands = hands; }

void 
NetPacketWinner::SetAmount(float amount)
{m_fAmnt=amount;}

std::string 
NetPacketWinner::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;potnum;rank;playerlist;cards;|
	//playerlist: pid/pid/pid
	std::string strData ="%d*%d*%d;%d;%d;%f;";
	
	//collect player list
	for(unsigned idx=0;idx<m_PlayerList.size();idx++){
		//cards format is "suite/vale;"
		std::string strPlayer = format("%d",
			m_PlayerList[idx]);
		
		if(idx+1 < 	m_PlayerList.size()){
			strPlayer += PACKET_INTERNAL_DELIMETER;
		}
		strData += strPlayer;
	}

	strData += PACKET_PARAMETER_DELIMETER;
	for(std::vector<Card>::iterator it = m_Hands.begin(); it != m_Hands.end(); it++)
	{
		//cards format is "suite/vale;"
		std::string strCard = format("%d/%d;",
			(*it).GetSuite(),
			(*it).GetFace());
			
		strData += strCard;
	}

	strData += PACKET_DELIMETER;
	std::string strRawPacket =
		format(strData.c_str(),
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_PotNum,
			(int)m_nRank,
			m_fAmnt);

	return strRawPacket;
}


/* Net Packet Show cards */
NetPacketShowHands::NetPacketShowHands(): 
	NetPacketRoom(),m_PlayerID(0)
{
	m_GamePacketID = eShowHands;
}

void 
NetPacketShowHands::SetPlayerID(IDType pID)
{ m_PlayerID =pID; } 

void 
NetPacketShowHands::SetHands(std::vector<Card>& hands)
{  m_Hands = hands; }

std::string 
NetPacketShowHands::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;playerID;cards;|
	std::string strData ="%d*%d*%d;%d;";

	for(std::vector<Card>::iterator it = m_Hands.begin(); it != m_Hands.end(); it++)
	{
		//cards format is "suite/vale;"
		std::string strCard = format("%d/%d;",
			(*it).GetSuite(),
			(*it).GetFace());
			
		strData += strCard;
	}

	strData += PACKET_DELIMETER;
	std::string strRawPacket =
		format(strData.c_str(),
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_PlayerID);

	return strRawPacket;	
}
/* chat message */
NetPacketChatMsg::NetPacketChatMsg() 
	: NetPacketRoom(), m_PlayerID(0),
	m_strChatMsg("")
{
	m_GamePacketID = eChatMsg;
}


void 
NetPacketChatMsg::SetMsg(const std::string& strMsg)
{ m_strChatMsg = strMsg; }

std::string 
NetPacketChatMsg::GetMsg() const
{ return m_strChatMsg;}

void 
NetPacketChatMsg::SetPlayerID(IDType pID)
{
	m_PlayerID = pID;
}

IDType 
NetPacketChatMsg::GetPlayerID() const
{ return m_PlayerID; }


std::string 
NetPacketChatMsg::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;msg;|
	std::string strRawPacket =
		format("%d*%d*%d;%d;%s;|",
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_PlayerID,
			m_strChatMsg.c_str());

	return strRawPacket;
}

/* open cards for seven card stud */
NetPacketOpenCards::NetPacketOpenCards(GamePacket id)
	: NetPacketRoom()
{
	m_GamePacketID = id;
	m_OwnedList.clear();
}
void 
NetPacketOpenCards::SetCardList(std::vector<CardOwn> list)
{
	m_OwnedList = list;
}

std::string 
NetPacketOpenCards::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;cardowner;..;|			
	//card owner is: playerID&cards1&cards;
	//cards is: suite/face
	std::string strData ="%d*%d*%d;";

	for(std::vector<CardOwn>::iterator it = m_OwnedList.begin(); 
		it != m_OwnedList.end(); it++)
	{
		std::string strCard ="";
		std::string strOwnerInfo = format("%d&",
			(*it).playerID);
		
		for(std::vector<Card>::iterator itc = (*it).hands.begin(); 
			itc != (*it).hands.end(); itc++)
		{
			//cards format is "suite/vale;"
			strCard = format("%d/%d",
				(*itc).GetSuite(),
				(*itc).GetFace());

			if( itc+1 != (*it).hands.end() ){
				strCard	+= PACKET_INTERNAL_DELIMTER_2;
			}
		}
		
		strCard += PACKET_PARAMETER_DELIMETER;
		strOwnerInfo += strCard;
		strData += strOwnerInfo;
	}

	strData += PACKET_DELIMETER;
	std::string strRawPacket =
		format(strData.c_str(),
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID);

	return strRawPacket;	
}


/* Custom Packet for 1 parameter(int) in Room */
NetPacketRoomInt::NetPacketRoomInt(GamePacket id)
	: NetPacketRoom()
{
	m_GamePacketID = id;
}

NetPacketRoomInt::NetPacketRoomInt(GamePacket id,int param)
	: NetPacketRoom(), m_nParam(param)
{
	m_GamePacketID = id;
}

std::string NetPacketRoomInt::ToString()
{
	//packet_id(space)gamepacketid(space)roomnum;param;|
	std::string strRawPacket =
		format("%d*%d*%d;%d;|",
			m_sPacketID,
			m_GamePacketID,
			m_nRoomID,
			m_nParam);

	return strRawPacket;
}