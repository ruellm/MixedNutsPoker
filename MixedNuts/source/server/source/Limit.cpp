#include <stdio.h>
#include <math.h>
#include "ILimit.h"

//fixed limit
FixedLimit::FixedLimit()
{
	m_fStake = 0.0f;
	m_pTable.reset();
}

bool 
FixedLimit::IsNextStake()
{
	TableState state = m_pTable->GetState();
	//texas and omaha holdem
	if (state == eTurnState ||
        state == eRiverState) return true;
    //seven card stud
    if (state == e5thStreetState ||
        state == e6thStreetState ||
        state == e7thStreetState) return true;
  
    return false;
}

void 
FixedLimit::Update()
{
	m_nRaiserCnt = 0;
}

bool 
FixedLimit::IsBetValid(float fBet,
					   PokerAction action,
					   Player* p)
{	
	if(action == eRaise ||
		(action== eAllIn && 
		fBet > m_pTable->GetCurrentPot()->currRaise)){
		m_nRaiserCnt++;
	}

	if(action == eAllIn) return true;
	if(m_nRaiserCnt >= MAX_RAISER_CNT ) return false;
	//if(fmod(fBet,GetCurrentStake())>0.0f) return false;	//disable this 03/12/2010
	return true;
}

float 
FixedLimit::GetCurrentStake()
{
	float stake = m_fStake;
	if(!IsNextStake())
		stake /= 2.0f;

	return stake;
}

//no limit
NoLimit::NoLimit()
{
	m_fStake = 0.0f;
	m_pTable.reset();
}

bool 
NoLimit::IsBetValid(float fBet,
					PokerAction action,
					Player* p)
{
	return true;
}

float 
NoLimit::GetCurrentStake()
{
	return m_fStake;
}

//pot limit
PotLimit::PotLimit()
{
	m_fStake = 0.0f;
	m_pTable.reset();
}

bool 
PotLimit::IsBetValid(float fBet,
					 PokerAction action,
					 Player* p)
{
	if(p==NULL) return true;
	if( fBet > GetLimit(p) ) return false;
	return true;
}

float 
PotLimit::GetCurrentStake()
{
	return m_fStake;
}

float 
PotLimit::GetLimit(Player* p)
{
	 float boardPot = 0.0f;
     float trailingPot = 0.0f;
     float totalPot = 0.0f;
     float potLimitRaise = 0.0f;
	 
	 for(int t=0;t<m_pTable->GetPotSize();t++)
	 {
		Pot* p = m_pTable->GetPotAt(t);
		boardPot += p->amnt;
	 }

	 for(int s=0;s<m_pTable->GetSeatCount();s++)
	 {
		Player* p = m_pTable->GetPlayerAt(s);
		if(!p || !p->IsPlaying()) continue;
		trailingPot += p->GetBet();
	 }

	 //definition of Pot Limit: Pot includes
	 //main pot + trailing pot + to call amount	 
	 Pot* currPot = m_pTable->GetCurrentPot();
	 float fNeedsToCall = currPot->currRaise - p->GetBet();
     if (currPot->currRaise == 0)
         fNeedsToCall = 0;

	 totalPot = boardPot + trailingPot + fNeedsToCall;
	 potLimitRaise = totalPot + fNeedsToCall;
	 return potLimitRaise;
}