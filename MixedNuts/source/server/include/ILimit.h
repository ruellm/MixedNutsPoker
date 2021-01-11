#ifndef _ILIMIT_H_
#define _ILIMIT_H_

#include <boost/shared_ptr.hpp>
#include "definitions.h"
#include "Table.h"

//raise count limit for fixed limit
#define MAX_RAISER_CNT 4

//limit type base
class ILimit
{
public:
   virtual bool IsBetValid(float fBet,
	   PokerAction action,Player* p = NULL) = 0;

   virtual float GetCurrentStake() = 0;
   virtual void Update(){}
   inline void SetStake(float stake){
		m_fStake = stake;
   }
   inline void SetTable(boost::shared_ptr<Table> pTable){
		m_pTable = pTable;
   }
protected:
	float m_fStake;
	//TODO: table should be refactored
	boost::shared_ptr<Table> m_pTable;
};

//fixed limit
class FixedLimit: public ILimit
{
public:
	FixedLimit();	
	virtual bool IsBetValid(float fBet,
		PokerAction action,Player* p = NULL);
	virtual float GetCurrentStake();
	virtual void Update();
private:
    int m_nRaiserCnt;
	bool IsNextStake();
};

//no limit
class NoLimit: public ILimit
{
public:
	NoLimit();
	virtual bool IsBetValid(float fBet,
		PokerAction action,Player* p = NULL);
	virtual float GetCurrentStake();
};

//pot limit
class PotLimit: public ILimit
{
public:
	PotLimit();
	virtual bool IsBetValid(float fBet,
		PokerAction action,Player* p = NULL);
	virtual float GetCurrentStake();
private:
	float GetLimit(Player* p);
};


#endif