#ifndef _SITANDGO_H_
#define _SITANDGO_H_


#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "PokerGameBase.h"
#include "IGameHandler.h"

class SitAndGo;
typedef void (SitAndGo::*SNGNFNPTR)();

class SitAndGo :
	public PokerGameBase,
	public IGameHandler
{
public:
	SitAndGo(GameIDType id);

	virtual void PlayNextRound();
	virtual void HandleWin();		
	virtual void BeginRound();		
	virtual void InternalInit();	
	virtual int GetNumPlayers();
	virtual void Start();
	virtual void TimerWakeUp();
	virtual void AddSession(boost::shared_ptr<ClientSession> session);		
	virtual void StandUpPlayer(boost::shared_ptr<ClientSession> session);
	virtual void SendPlayerList( boost::shared_ptr<ClientSession> session ); //send layer list
	virtual void SendGameInfo(boost::shared_ptr<ClientSession> session );
	virtual void LeaveRoom(boost::shared_ptr<ClientSession> session);
	virtual void SendTableSnapShot();

	virtual void HandlePacket(boost::shared_ptr<NetPacketRoom> packet,
		boost::shared_ptr<ClientSession> session);

	virtual void SeatDownPlayer(boost::shared_ptr<NetPacketJoinGame> packet, 
		boost::shared_ptr<ClientSession> session);	
	
	IDType GetID() const;
	void SetGame(boost::shared_ptr<PokerGameBase> game);				

private:
	boost::asio::deadline_timer m_BlindsTimer;
	SNGNFNPTR m_pfnNextState;
	boost::shared_ptr<PokerGameBase> m_pGame;
	float m_fNextStake;
	int m_nLevel;

	virtual bool HasRoundPlayers();
	void HandlePostGame();
	void StartBlindTimer();
	void TimerExpired(const boost::system::error_code &ec);

};

#endif