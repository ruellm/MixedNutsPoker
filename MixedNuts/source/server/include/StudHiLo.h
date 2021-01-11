#ifndef _STUDHILO_H_
#define _STUDHILO_H_

#include "SevenCardStud.h"
#include "HighLowBase.h"
class StudHiLo: 
	public SevenCardStud,
	public HighLowBase
{
public:
	//StudHiLo(void);
	StudHiLo(GameIDType id);
	~StudHiLo(void);
	virtual void ShowDown();
};

#endif
