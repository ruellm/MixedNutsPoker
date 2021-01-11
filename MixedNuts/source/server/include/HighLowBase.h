#ifndef _HIGHLOWBASE_H_
#define _HIGHLOWBASE_H_

#include <vector>
#include "Table.h"
#include "PacketSenderGame.h"

class HighLowBase
{
public:
	HighLowBase(void);
	~HighLowBase(void);
protected:
	void DeclareWinnerHighLow(	std::map<IDType,HandStrength> highHands,
							std::map<IDType,HandStrength> lowHands,
							boost::shared_ptr<Table> pGameTable,
							boost::shared_ptr<PacketSenderGame> pSender,
							DataManager< boost::shared_ptr<ClientSession> >&	clientList);

};

//low ball games variant - based class
class LowBallBase
{
public:
	LowBallBase(){}
	~LowBallBase(){}
protected:
	void DeclareWinnerLow(std::map<IDType,HandStrength> playingHands,
							boost::shared_ptr<Table> pGameTable,
							boost::shared_ptr<PacketSenderGame> pSender,
							DataManager< boost::shared_ptr<ClientSession> >&	clientList);

};

#endif
