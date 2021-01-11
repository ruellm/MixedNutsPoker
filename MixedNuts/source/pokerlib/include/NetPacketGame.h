#ifndef _NETPACKETGAME_H_
#define _NETPACKETGAME_H_

#include <vector>
#include "INetPacket.h"
#include "Card.h"
#include "Table.h"
#include "PokerHandsLogic.h"

/* Room specific base */
class NetPacketRoom: 
	public INetPacket
{
public:
	NetPacketRoom();
	NetPacketRoom(GamePacket id):
		m_GamePacketID(id){}
	IDType GetRoomID() const;
	GamePacket GetCommand() const;
	void SetRoom(IDType room);
	virtual std::string ToString();	
protected:
	GamePacket m_GamePacketID;
	IDType	m_nRoomID;
};

/* Join Game */
class NetPacketJoinGame : 
	public NetPacketRoom
{
public:
	NetPacketJoinGame(void);
	virtual std::string ToString();	
	short  GetSeatNum() const;    
	float  GetBuyIn() const;
	void SetSeatNum(short seatno);
	void SetBuyIn(float amt);
private:

	short m_nSeatNum;
	float m_fBuyIn;
};

/* Hole cards */
class NetPacketCards: 
	public NetPacketRoom
{
public:
	NetPacketCards(GamePacket id,IDType room,const std::vector<Card>& card);
	virtual std::string ToString();
private:
	std::vector<Card> m_Cards;
};

/* Table snap shot - Poker common */
class NetPacketPokerTableSnapShot:
	public NetPacketRoom
{
public:
	NetPacketPokerTableSnapShot(boost::shared_ptr<Table> table,IDType room);
	virtual std::string ToString();
private:
	boost::shared_ptr<Table> m_pTable;

};

/* Table snap shot - Holdem */
//class NetPacketHoldemTableSnapShot:
//	public NetPacketRoom
//{
//public:
//	NetPacketHoldemTableSnapShot(boost::shared_ptr<TableHoldem> table,IDType room);
//	virtual std::string ToString();
//private:
//	boost::shared_ptr<TableHoldem> m_pTable;
//
//};

/* Wait action*/
/*TODO: TBD. strucuture of this class not final*/
class NetPacketWaitBet: 
	public NetPacketRoom
{
public:
	NetPacketWaitBet(IDType room);
	virtual std::string ToString();
};

/* Net packet action */
class NetPacketAction:
	public NetPacketRoom
{
public:
	NetPacketAction(IDType room);	
	NetPacketAction(PokerAction action, float amnt,IDType room/*,short seatnum*/);	
	PokerAction GetAction() const;
	float GetAmount() const;
	void SetAction(PokerAction action);
	void SetAmount(float amnt);
	//short GetSeatNum() const;
	//void SetSeatNum(IDType val);
	virtual std::string ToString();	

private:
	PokerAction m_Action;
	float m_fAmount;
	//IDType m_nSeat;
};


/* Net Packet declaring the winner */
class NetPacketWinner:
	public NetPacketRoom
{
public:
	NetPacketWinner();
	virtual std::string ToString();
	void SetPlayerList(std::vector<IDType> players);
	void SetPotNum(short potnum);
	void SetHandRank(HandStrength::HandRanking rank);
	void SetHands(std::vector<Card>& hands);
	void SetAmount(float amount);
private:
	std::vector<IDType> m_PlayerList; //playerlist	
	short m_PotNum;		//Pot ID
	float m_fAmnt;
	HandStrength::HandRanking m_nRank;		//from rank enum, 100 means nothing, won by fold
	std::vector<Card>  m_Hands;
};

/* Net Packet Show cards */
class NetPacketShowHands:
	public NetPacketRoom
{
public: 
	NetPacketShowHands();
	virtual std::string ToString();
	void SetPlayerID(IDType pID);
	void SetHands(std::vector<Card>& hands);
private: 
	IDType m_PlayerID;	//owner of hand
	std::vector<Card>  m_Hands;
};


/* chat message */
class NetPacketChatMsg: 
	public NetPacketRoom
{
public: 
	NetPacketChatMsg();
	virtual std::string ToString();
	void SetMsg(const std::string& strMsg);
	std::string GetMsg() const;
	void SetPlayerID(IDType pID);
	IDType GetPlayerID() const;
private:
	std::string m_strChatMsg;
	IDType m_PlayerID;
};

/* open cards for seven card stud */
class NetPacketOpenCards:
	public NetPacketRoom
{
public:		
	NetPacketOpenCards(GamePacket id = eOpenCards);
	virtual std::string ToString();
	void SetCardList(std::vector<CardOwn> list);
private:
	std::vector<CardOwn> m_OwnedList;
};


/* Custom Packet for 1 parameter(int) in Room */
class NetPacketRoomInt:
	public NetPacketRoom
{
public:
	NetPacketRoomInt(GamePacket id);
	NetPacketRoomInt(GamePacket id,int param);
	virtual std::string ToString();
private:
	int m_nParam;
};

#endif