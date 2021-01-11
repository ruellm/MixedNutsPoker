#ifndef _GAMECONTROL_H_
#define _GAMECONTROL_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "definitions.h"
#include "ClientSession.h"
#include "Table.h"
#include "Deck.h"
#include "PacketSenderGame.h"
#include "DataManager.h"
#include "NetPacketGame.h"
//#include "Pot.h"

class GameBase
{
public:
	GameBase(void);
	GameBase(GameIDType id);	
	GameBase(GameIDType id,	const GameInfo& gameInfo);
	virtual ~GameBase(void);

	virtual void Init();
	virtual void Start() = 0;			
	virtual int GetNumPlayers() { return 0; }
	virtual void AddSession(boost::shared_ptr<ClientSession> session);	
	virtual void StandUpPlayer(boost::shared_ptr<ClientSession> session);
	virtual void SendPlayerList( boost::shared_ptr<ClientSession> session ); //send layer list
	virtual void SendGameInfo(boost::shared_ptr<ClientSession> session );
	virtual void LeaveRoom(boost::shared_ptr<ClientSession> session);

	IDType GetID() const;
	boost::asio::deadline_timer& GetStateTimer();	
	boost::shared_ptr<GameInfo> GetGameInfo() const;
	boost::shared_ptr<Table> GetGameTable() const;		
	void SetGameInfo(boost::shared_ptr<GameInfo> gInfo);		
	virtual void HandlePacket(boost::shared_ptr<NetPacketRoom> packet,
		boost::shared_ptr<ClientSession> session);
	
	virtual void SeatDownPlayer(boost::shared_ptr<NetPacketJoinGame> packet, 
		boost::shared_ptr<ClientSession> session);	

protected:
	boost::asio::deadline_timer m_gameTimer; //use expires_from_now()	
	GameIDType					m_gameID;	
	boost::recursive_mutex		m_SessionMutex;
	ClientSessionList			m_SubscribedPlayerList;
	boost::shared_ptr<Table>	m_pGameTable;
	boost::shared_ptr<GameInfo> m_pGameInfo;
	Deck						m_Deck;
	unsigned					m_iHandCount;	
	boost::shared_ptr<PacketSenderGame>  m_pPacketSender;
	DataManager< boost::shared_ptr<ClientSession> >	m_ClientList;
	
	void WaitForTimer(unsigned msecs);	
	void WaitForTimerSync(unsigned msecs);
	void TimerTrigger(const boost::system::error_code &ec);

	virtual bool HasValidPlayers( );
	virtual bool HasRoundPlayers( );	//different game flavors might have a different way of handling players
	virtual void NextState() {};		//must implement, abstract?
	virtual void TimerWakeUp(){};		//called when timer waiks after call to wait
	virtual void HandleWin() = 0;		//handle winner in game, based on game mode and game flavor
	virtual void BeginRound() = 0;		//Compute and prepare next round
	virtual void InternalInit() = 0;	//Initialize for child classes
	virtual void SendTableSnapShot(){}	//send table snap shot to all
	virtual bool InternalHandlePacket(boost::shared_ptr<NetPacketRoom> packet,
		boost::shared_ptr<ClientSession> session) = 0;

};

#endif
