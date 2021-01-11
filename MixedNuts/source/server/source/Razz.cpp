#include "Razz.h"
#include "PokerHandsLogic.h"
#include "GameTimeoutDef.h"

//Razz::Razz(void)
//{
//}
Razz::Razz(GameIDType id):
	SevenCardStud(id)
{
	//The difference in Hi-Lo is 
	//that the bets cannot be doubled on the fourth street - LowBall
	m_bProtectPair = false;
}
Razz::~Razz(void)
{
}

void Razz::ShowDown()
{
	//lowball rules lowest hand wins
	PokerHandsLogic logic;
	std::map<IDType,HandStrength> hands;

	//create player hands for remaining players
	for(int idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player || player->GetAction() == eFold || 
			!player->IsPlaying()) continue;
		
		HandStrength hand = logic.GetLowestHand(player->GetHoleCards(),
			player->GetID());

		hands.insert(std::map<IDType, HandStrength>::value_type(player->GetID(),hand));		
	}
	
	//declare winner low hand
	DeclareWinnerLow(hands,m_pGameTable,
		m_pPacketSender,m_ClientList);					

	//delay for winner declaration
	WaitForTimerSync(SERVER_DELAY_DECLARE_WINNER);

	if(m_pGameHandler)
		m_pGameHandler->HandlePostGame();
}

int Razz::GetBringInSeat()
{
	//the highest exposed card after the deal "bring-in" the action
	//this function is only used during "bring-in"
	int index = 0;
	const int thirdStreet = 2;

	Card highest;

	highest.SetFace(Card::Ace);
	highest.SetSuite(Card::Clubs);

	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( !m_pGameTable->IsSeatPlaying(idx) ) continue;
		Player* p = m_pGameTable->GetPlayerAt(idx);
		
		if( p->GetAction() == eFold || 
			p->GetAction() == eAllIn) continue;

		Card c = p->GetHoleCards()[thirdStreet];
		if( (c > highest && 
			c.GetFace() != Card::Ace) ||
			highest.GetFace() == Card::Ace && c.GetFace() > 1){
			highest = c;
			index = idx;
		}
		
		else if( c == highest)
		{
			//compare by suite
			if(c.GetSuite() > highest.GetSuite()){
				highest = c;
				index = idx;
			}
		}
	}
	return index;
}

int Razz::GetDealerSeat()
{
	PokerHandsLogic logic;
	std::vector<HandStrength> players;
	std::vector<HandStrength> winers;
	//the lowest hand not the highest starts the betting, 
	std::vector<CardOwn> ownerList = GetOpenCards();	
	
	//generate lowest hand strength for all players
	for(unsigned c=0;c<ownerList.size();c++){
		CardOwn owner = ownerList[c];
		HandStrength hands =  
			logic.CreateHandLowBall(owner.hands,owner.playerID);
		players.push_back(hands);
	}

	//get winning hand
	logic.GetWinningHand(players,winers,true);	
	
	int closest_dist = -1;
	short seatno = m_pGameTable->GetDealer();
	for(unsigned z=0;z<winers.size();z++){
		//if player hands are same,
		//player close to the dealer's left bets						
		Player* p = 
			m_pGameTable->GetPlayer(winers[z].GetPlayerID());
		if(!p) continue;			
		short player_seat = p->GetSeatNum();
		short distance = 0;
		if(player_seat < m_pGameTable->GetDealer())
			player_seat += m_pGameTable->GetSeatCount();
		distance = player_seat - m_pGameTable->GetDealer();				
		
		if(closest_dist == -1){
			closest_dist = distance;
			seatno = p->GetSeatNum();
			continue;
		}else if(distance < closest_dist){
			distance = closest_dist;
			seatno = p->GetSeatNum();
		}
	}
	
	return seatno;	

}
