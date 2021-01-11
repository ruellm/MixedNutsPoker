#ifndef _RAZZ_H_
#define _RAZZ_H_

#include "SevenCardStud.h"
#include "HighLowBase.h"

class Razz: 
	public SevenCardStud,
	public LowBallBase
{
public:
	//Razz(void);
	Razz(GameIDType id);
	~Razz(void);
	virtual void ShowDown();
private:
	virtual int GetBringInSeat();
	virtual int GetDealerSeat();
};

#endif