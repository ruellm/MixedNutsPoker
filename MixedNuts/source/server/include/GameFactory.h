#ifndef _GAMEFACTORY_H_
#define _GAMEFACTORY_H_

#include <boost/shared_ptr.hpp>
#include "definitions.h"
#include "GameBase.h"

class GameFactory
{
public:
	GameFactory(void);
	~GameFactory(void);
	void CreateLimit(boost::shared_ptr<PokerGameBase> poker_game);
	boost::shared_ptr<GameBase>  CreateGame(boost::shared_ptr<GameInfo> gInfo);	
	
	IDType GetCurrentID( ) const{
		return m_lLastGameID;
	}

private:
	IDType m_lLastGameID;
};

#endif