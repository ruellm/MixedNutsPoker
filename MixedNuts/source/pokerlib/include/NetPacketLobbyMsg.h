#ifndef _NETPACKETLOBBYMSG_H_
#define _NETPACKETLOBBYMSG_H_

#include <string>
#include "definitions.h"
#include "INetPacket.h"
#include "Player.h"

/*
 * Request room common
*/
class NetPacketReqRoomList: 
	public INetPacket
{
public:
	NetPacketReqRoomList()
	{
		m_sPacketID = eReqGameList;
	}
	inline void SetGameType( GameType type ){
		m_gameType = type;
	}
	inline GameType GetGameType() const {
		return m_gameType;
	}
	virtual std::string ToString();
protected:
	GameType m_gameType;
};

/*
* Request game room POKER list base
*/
class NetPacketReqRoomPokerList: 
	public NetPacketReqRoomList
{
public:
	NetPacketReqRoomPokerList(){		
		m_sPacketID = eReqPokerGameList;
	}

	inline void SetGameMode( GameModeType mode ){
		m_gameMode = mode;
	}
	inline GameModeType GetGameMode() const {
		return m_gameMode;
	}

	inline void SetGameLimit( GameLimitType limit ){
		m_gameLimit = limit;
	}
	inline GameLimitType GetGameLimit() const {
		return m_gameLimit;
	}
	
	virtual std::string ToString();
protected:
	GameModeType m_gameMode;
	GameLimitType m_gameLimit;
	GameType m_gameType;
};

/*
 * Create Game
*/
class NetPacketCreateRoom : 
	public INetPacket
{
public: 
	NetPacketCreateRoom(){
		m_sPacketID = eCreateGame;
	}
	NetPacketCreateRoom(boost::shared_ptr<GameInfo> gInfo): 
		m_pGameInfo(gInfo){
		m_sPacketID = eCreateGame;
	}
	void SetGameInfo(boost::shared_ptr<GameInfo> gInfo){
		m_pGameInfo = gInfo;
	}
	
	boost::shared_ptr<GameInfo>	GetGameInfo() const	{
		return m_pGameInfo;
	}
	virtual std::string ToString();
protected:
	boost::shared_ptr<GameInfo> m_pGameInfo;
};

/*
 * Create Game - POKER base
 */

class NetPacketCreateRoomPoker: 
	public NetPacketCreateRoom
{
public:
	NetPacketCreateRoomPoker(){
		m_sPacketID = eCreatePokerGame;
	}
	NetPacketCreateRoomPoker(boost::shared_ptr<GameInfo> gInfo): NetPacketCreateRoom(gInfo){}
	virtual std::string ToString();
	
};

// Game list item
class NetPacketGameListItem: 
	public INetPacket
{
public:
	NetPacketGameListItem()	{
		m_sPacketID = eGameListItem;
	}
	virtual std::string ToString();
	inline void SetGameID(IDType id){
		m_gameID = id;
	}
	IDType GetGameID() const{
		return m_gameID;
	}
	void SetGameDesc(std::string strDesc){
		m_strGameDesc = strDesc;
	}
	void SetStake(float stake){
		m_fStake = stake;
	}

	void SetGameType(GameType type){
		m_gameType = type;
	}
	void SetNumPlayers(int nump){
		m_nNumPlayers = nump;
	}
	void SetMaxPlayers(int max){
		m_nMaxPlayers = max;
	}
protected:
	IDType m_gameID;
	std::string m_strGameDesc;
	float m_fStake;
	GameType m_gameType;
	int m_nNumPlayers;
	int m_nMaxPlayers;
};

// Game list item - Poker
class NetPacketPokerGameListItem:
	public NetPacketGameListItem
{
public:
	NetPacketPokerGameListItem():
	  NetPacketGameListItem(),
	  m_fAntes(0.0f),m_fBringIn(0.0f),m_nRaiseTime(0),
	  m_fRaiseFactor(0.0f){}

	virtual std::string ToString();
	
	void SetGameLimit(GameLimitType limit){
		m_gameLimit = limit;
	}
	void SetAntes(float value){
		m_fAntes = value;
	}
	void SetBringIn(float value){
		m_fBringIn = value;
	}
	void SetRaiseFactor(float value){
		m_fRaiseFactor = value;
	}
	void SetRaiseTime(int value){
		m_nRaiseTime = value;
	}

protected:
	GameLimitType m_gameLimit;
	float m_fAntes;
	float m_fBringIn;
	unsigned m_nRaiseTime;
	float m_fRaiseFactor;
};

// Game list item - seven card stud Poker
//class NetPacketSevenCardStudGameListItem:
//	public NetPacketPokerGameListItem
//{
//public:
//	NetPacketSevenCardStudGameListItem():NetPacketPokerGameListItem(){}
//	virtual std::string ToString();
//	void SetAntes(float value){
//		m_fAntes = value;
//	}
//	void SetBringIn(float value){
//		m_fBringIn = value;
//	}
//protected:
//	float m_fAntes;
//	float m_fBringIn;
//};


//Packet for integer parameters 
class NetPacketInt: public INetPacket
{
public:
	NetPacketInt(PacketIDType type,IDType param);
	virtual std::string ToString();
	IDType GetParam() const;
private:	
	IDType m_nParam;
};

/* Packet for request logedin */
class NetPacketReqLogIn: public INetPacket
{
public:
	NetPacketReqLogIn();
	virtual std::string ToString();
	std::string GetName() const;
	std::string GetPassWord() const;
	void SetUserName(std::string uname);
	void SetPassword(std::string pass);
private:
	std::string m_strUserName;
	std::string m_strPassword;
};

/* Packet Client request feedback */
class NetPacketClientInfoFeedBack: 
	public INetPacket
{
public:
	NetPacketClientInfoFeedBack();
	virtual std::string ToString();
	std::string GetName() const;
	void SetName(std::string uname);
	std::string GetLocation() const;
	void SetLocation(std::string loc);
	void SetPlayerID(IDType pID);
	IDType GetPlayerID() const;
private:
	std::string m_strUserName;
	std::string m_strLocation;
	IDType m_nPlayerID;
};

/* game Player list */
class NetPacketPlayerList: 
	public INetPacket
{
public:
	NetPacketPlayerList();
	virtual std::string ToString();
	void AddPlayer(Player* p);
	void SetPlayers(std::vector<Player*>& list);
	void SetRoomID(IDType id);
private:
	std::vector<Player*> m_PlayerList;
	IDType m_nRoomID;
};

#endif