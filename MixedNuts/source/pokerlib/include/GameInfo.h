#ifndef _GAMEINFO_H_
#define _GAMEINFO_H_

#include <string>

#define GAMEDESCMAX   20

//blinds information
typedef struct 
{
	unsigned raise_time;	//in seconds
	float raise_factor;		//in floating point
}BlindsInfo;

//Game room information base
struct GameInfo{
	GameType game_type;				 // game flavors
	float stake;					 // game room max stake,
									 // in poker big blind in cash game, buyin in SNG and tournament 		
	int max_players;				 // max players
	int action_time_imit;			 // player action time limit in secods				
									 //TODO: add game table owner id
									 // TODO game description
	std::string strDescription;		 // game little info, displayed in client

	//default constructor
	GameInfo()
	{
		max_players = 0;
		action_time_imit=0;
		stake = 0.0f;
	}
};

//Game info for poker specific game
struct GameInfoPoker: GameInfo {
	GameModeType mode;				// game mode
	GameLimitType limit_type;		// pot limit type
	
	//for 7 card stud
	float fAntes;				//forced antes
	float fBringIn;				//bring in

	//for SNG games
	BlindsInfo blindsInfo;		//blinds info
	float startingChips;		//starting chips for SNG and tournaments

	GameInfoPoker()
	{
		fAntes = 0.0f;
		fBringIn = 0.0f;
		startingChips = 0.0f;
		blindsInfo.raise_time = 0;
		blindsInfo.raise_factor = 0.0f;
	}
};

#endif
