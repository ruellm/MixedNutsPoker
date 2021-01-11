#ifndef _POKERGAMEBASE_H_
#define _POKERGAMEBASE_H_

#include <boost/enable_shared_from_this.hpp>
#include "GameBase.h"
#include "PacketSenderGame.h"
#include "NetPacketGame.h"
#include "ILimit.h"
#include "IGameHandler.h"

class PokerGameBase: 
	public GameBase,
	public boost::enable_shared_from_this<PokerGameBase>	
{
public:
	//PokerGameBase(void);
	PokerGameBase(IDType id);
	PokerGameBase(GameIDType id,const GameInfo& gameInfo);
	void SetGameHandler(IGameHandler* handler);
	void SetLimit(boost::shared_ptr<ILimit> limit);	
	
	virtual ~PokerGameBase(void);
	virtual int GetNumPlayers();		
	virtual void SendGameInfo(boost::shared_ptr<ClientSession> session );		
	virtual void PlayNextRound()=0;
	virtual void SendTableSnapShot();

protected:
	void ShowHandAll();
	void ResetPotRaise();
	void DealHole(int count,short from=0);
	void DealCardToAll(int count, short from = 0,GamePacket id = eHoleCard,bool bSend=false);
	void BettingRound(bool newRound=false,bool newHand=false);
	void StartBettingRound(bool newHand=false);
	void ShowPlayerHandToAll(IDType id);

	virtual void NextState();
	virtual void InternalInit();
	virtual void HandleAllFold();			
	virtual void NextBettingRound();	
	virtual void HandleWin();
	virtual void DeclareWinner(std::map<IDType,HandStrength>& playerHands);
	virtual void HandleGameProcess(PokerAction action, float fBet){}
	
	virtual void HandleAction(boost::shared_ptr<NetPacketAction> packet,
		boost::shared_ptr<ClientSession> session);	

	virtual bool InternalHandlePacket(boost::shared_ptr<NetPacketRoom> packet,
		boost::shared_ptr<ClientSession> session);		

private:
	void TimerExpired(const boost::system::error_code &ec);
	void EndRound();

protected:
	bool m_bShowHands;
	boost::shared_ptr<ILimit> m_pLimit;
	boost::shared_ptr<GameInfoPoker> m_pPokerGameInfo;
	IGameHandler* m_pGameHandler;
};

#endif