#ifndef _OMAHAHILO_H_
#define _OMAHAHILO_H_

#include "OmahaHoldem.h"
#include "HighLowBase.h"

class OmahaHiLo:
	public OmahaHoldem,
	public HighLowBase
{
public:
	//OmahaHiLo(void);
	OmahaHiLo(GameIDType id);
	~OmahaHiLo(void);
	virtual void ShowDown();
};

#endif