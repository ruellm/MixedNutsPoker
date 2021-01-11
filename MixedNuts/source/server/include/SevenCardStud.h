#ifndef _SEVENCARDSTUD_H_
#define _SEVENCARDSTUD_H_

#include "PokerGameBase.h"

class SevenCardStud;
typedef void (SevenCardStud::*SCSTDNFNPTR)();

class SevenCardStud :
	public PokerGameBase
{
public:
	//SevenCardStud(void);
	SevenCardStud(GameIDType id);
	virtual ~SevenCardStud(void);
	virtual void InternalInit();
	virtual void Start();
	virtual void BeginRound();
	virtual void TimerWakeUp();
	/*virtual void SendTableSnapShot();	*/
	virtual void AddSession(boost::shared_ptr<ClientSession> session);	
	//virtual void SendGameInfo(boost::shared_ptr<ClientSession> session );

protected:
	virtual void PlayNextRound();
	virtual void InternalDealHole();			
	virtual void HandleGameProcess(PokerAction action, float fBet);
	virtual void ShowDown();
	virtual void HandleWin();	
	virtual int GetBringInSeat();
	virtual int GetDealerSeat();
	
	bool m_bProtectPair;
	std::vector<CardOwn> GetOpenCards();

private:
	bool m_b4thStreetRaise;		
	TableState m_nCurrentState;

	SCSTDNFNPTR m_pfnNextState;
	void SendOpenCard(GamePacket id);
	void SendAllOpenCards();
	int GetOpenCardsCount();
	void FourthStreet();
	bool IsTherePairOnBoard();	
	void FifthStreet();
	void SixthStreet();
	void SeventhStreet();	
};

#endif