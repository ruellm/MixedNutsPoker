#ifndef _TEXASHOLDEM_H_
#define _TEXASHOLDEM_H_

#include "PokerGameBase.h"
#include "Card.h"

//forward declaration
class TexasHoldem;
typedef void (TexasHoldem::*TXHFNPTR)();

class TexasHoldem: 
	public PokerGameBase
{
public:
	/*TexasHoldem(void);*/
	~TexasHoldem(void);
	TexasHoldem(GameIDType id);
	TexasHoldem(GameIDType id,const GameInfo& gameInfo);
	/*virtual void InternalInit();*/
	virtual void Start();	
	/*virtual void NextState();*/
	virtual void TimerWakeUp();
	virtual void HandleWin();
	//virtual void SendTableSnapShot();	
	virtual void AddSession(boost::shared_ptr<ClientSession> session);	
	virtual void HandleGameProcess(PokerAction action, float fBet);
	virtual void DealFlop();

protected:
	virtual void InternalDealHole();
	TXHFNPTR  m_pfnNextState;
	std::vector<Card>  m_CommunityCards;

private:	
	virtual void PlayNextRound();		
	void DealTurn();
	void DealRiver();	
	virtual void BeginRound();
	virtual void ShowDown();
	virtual void NextBettingRound();	
};

#endif