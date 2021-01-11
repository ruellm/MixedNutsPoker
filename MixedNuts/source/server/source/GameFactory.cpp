
#include "GameFactory.h"
#include "TexasHoldem.h"
#include "SevenCardStud.h"
#include "OmahaHoldem.h"
#include "StudHiLo.h"
#include "OmahaHiLo.h"
#include "Razz.h"
#include "SitAndGo.h"
#include "ILimit.h"

GameFactory::GameFactory(void):
	m_lLastGameID(0)
{
}

GameFactory::~GameFactory(void)
{
}

boost::shared_ptr<GameBase>  
GameFactory::CreateGame(boost::shared_ptr<GameInfo> gInfo)
{
	boost::shared_ptr<GameBase> game;

	switch(gInfo->game_type)
	{
		case eTexasHoldem:			
			game.reset( new TexasHoldem(m_lLastGameID++));												
			break;
		case eSevenCardStud:
			game.reset( new SevenCardStud(m_lLastGameID++));						
			break;
		case eOmahaHoldem:
			game.reset( new OmahaHoldem(m_lLastGameID++));
			break;
		case eSevenCardStudHiLo:
			game.reset( new StudHiLo(m_lLastGameID++));
			break;
		case eRazz:
			game.reset( new Razz(m_lLastGameID++));
			break;
		case eOmahaHiLo:
			game.reset( new OmahaHiLo(m_lLastGameID++));
			break;		
	}
	
	if(game)
		game->SetGameInfo (gInfo);

	//TODO: what is the correct sequence?
	if(ISPOKERGAME(gInfo->game_type))
	{
		boost::shared_ptr<GameInfoPoker> pPokerGameInfo = 
			boost::static_pointer_cast<GameInfoPoker>(gInfo);
		if(pPokerGameInfo->mode == eModeSNG && game)
		{			
			boost::shared_ptr<GameBase> sng(new SitAndGo(game->GetID()));
			boost::shared_ptr<SitAndGo> sngPtr = boost::dynamic_pointer_cast<SitAndGo>(sng);
			sngPtr->SetGameInfo (gInfo);
			sngPtr->SetGame(boost::dynamic_pointer_cast<PokerGameBase>(game));
			return sng;
		}
	}
		
	return game;
}

void 
GameFactory::CreateLimit(boost::shared_ptr<PokerGameBase> poker_game)
{
	boost::shared_ptr<ILimit> limit;
	boost::shared_ptr<GameInfoPoker> gInfoPoker = 
		boost::static_pointer_cast<GameInfoPoker>(poker_game->GetGameInfo());
	
	switch(gInfoPoker->limit_type)
	{
		case eNoLimit:{
			limit.reset( new NoLimit() );
			 }break;
		case ePotLimit:{
			limit.reset( new PotLimit() );
			 }break;
		case eFixedLimit:{
			limit.reset( new FixedLimit() );
			 }break;
	}

	//return if null
	//TODO: set default limit, nolimit or fixed?
	if(limit == NULL ) return;

	//set limit parameters
	limit->SetTable( poker_game->GetGameTable() );
	limit->SetStake( gInfoPoker->stake );

	//set limit
	poker_game->SetLimit(limit);
}
