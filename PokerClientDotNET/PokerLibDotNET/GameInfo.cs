using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace PokerLibDotNET
{
   //blinds information
    public class BlindsInfo
    {
	    public int raise_time;	//in milliseconds
        public float raise_factor;		//in floating point
							//TODO: antes?
    };

    public class GameInfo {
        public GameType game_type;				 // game flavors
        public float stake;					 // game room max stake,
        // in poker big blind in cash game, buyin in SNG and tournament 		
        public int max_players;			 // max players
        public int action_time_imit;		 // player action time limit in secods				
        //TODO: add game table owner id
        // TODO game description
        public string strDescription;		 // game little info, displayed in client
    
    }

    public class GameInfoPoker : GameInfo {
        public GameModeType mode;				// game mode
        public GameLimitType limit_type;		// pot limit type
        public float fAntes;				//forced antes
        public float fBringIn;				//bring in       
        public BlindsInfo blindsInfo;			//TODO: TBD if this is common to poker games
        public float fStartChips;
        public GameInfoPoker() {
            blindsInfo = new BlindsInfo();
        }
    }

    //for s3ven card stud
    //public class GameInfoSevenStud : GameInfoPoker
    //{
    //    public float fAntes;				//forced antes
    //    public float fBringIn;				//bring in
    //    public GameInfoSevenStud()
    //        : base()
    //    { 
            
    //    }
    //}
}
