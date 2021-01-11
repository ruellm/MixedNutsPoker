#ifndef _OMAHAHOLDEM_H_
#define _OMAHAHOLDEM_H_

#include "TexasHoldem.h"

class OmahaHoldem: 
	public TexasHoldem
{
public:
	/*OmahaHoldem(void);*/
	OmahaHoldem(GameIDType id);
	~OmahaHoldem(void);
	virtual void InternalDealHole();
	virtual void ShowDown();
};

#endif