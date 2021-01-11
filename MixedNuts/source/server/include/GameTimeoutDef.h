#ifndef _GAMETIMEOUTDEF_H_
#define _GAMETIMEOUTDEF_H_

#define SERVER_GAME_START_ROUND 500		//Delay before starting a round .5 second
#define SERVER_GAME_DEAL_HOLE	1000	//delay for deal hole 1 sec
#define SERVER_GAME_DEAL_FLOP   1000	//delay 1 sec before flop
#define SERVER_GAME_DEAL_TURN   1000	//delay 1 sec before flop
#define SERVER_GAME_DEAL_RIVER   1000	//delay 1 sec before flop
#define SERVER_TIMER_BETTING	500		//0.5 sec delay for betting round
#define SERVER_NEXT_PACKE_DELAY  1000
#define SERVER_DEFAULT_STATE_DELAY 1000 //delay per game state
#define SERVER_DELAY_FOR_WINNER 2000	//delay to declare 1 winner when all folded
#define SERVER_DELAY_DECLARE_WINNER 3000	//delay to declare winner(s) after showdown
#define SERVER_DELAY_SHOWDOWN_AFTERMATH 3000	//show down after math delay to let players analize what happen
#define SERVER_DEAL_STREET	1000
#define SERVER_DELAY_PACKET 500	//delay the process so that client can process the packet

//hack solution
#define SERVER_INFINITE_WAIT	(3600*24)   //wait for whole day
#endif