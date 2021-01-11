#include "OmahaHiLo.h"
#include "GameTimeoutDef.h"

//OmahaHiLo::OmahaHiLo(void)
//{
//}

OmahaHiLo::OmahaHiLo(GameIDType id):
	OmahaHoldem(id)
{
}


OmahaHiLo::~OmahaHiLo(void)
{
}

void OmahaHiLo::ShowDown()
{
	PokerHandsLogic logic;
	std::map<IDType,HandStrength> handsHigh;
	std::map<IDType,HandStrength> handsLow;

	//community card group per 3
	std::vector< std::vector<Card> > community_sets = 
		GroupCardByThree(m_CommunityCards);
	
	for(int idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player || player->GetAction() == eFold || 
			!player->IsPlaying()) continue;

		//hand list of player
		std::vector<HandStrength> handListH;
		std::vector<HandStrength> handListL;
		std::vector<HandStrength> winnerH;
		std::vector<HandStrength> winnerL;

		//generate player set cards
		std::vector< std::vector<Card> > player_sets = 
			GroupCardByTwo(player->GetHoleCards());

		//generate hand strength from community sets and player set
		for(unsigned ps=0;ps<player_sets.size();ps++){
			for(unsigned cs=0;cs<community_sets.size();cs++){
				HandStrength hands =  logic.CreateHandStrength(community_sets[cs],
					player_sets[ps],player->GetID());
				handListH.push_back(hands);

				std::vector<Card> low_cards;		
				std::vector<Card> combined;
				combined.insert(combined.end(),community_sets[cs].begin(),community_sets[cs].end());
				combined.insert(combined.end(),player_sets[ps].begin(),player_sets[ps].end());
				if(logic.IsHandQualifyLow(combined,low_cards))
				{
					HandStrength hand_low;
					hand_low.Cards = low_cards;
					hand_low.SetPlayerID(player->GetID());
					hand_low.SetRanking(HandStrength::LowHand);
					handListL.push_back(hand_low);
				}					
			}
		}

		//generate best hand for player
		logic.GetWinningHand(handListH,winnerH);
		if(handListL.size()>0)
			logic.GetWinningHand(handListL,winnerL,true);

		//insert the winning hand for the player
		//TODO: tied is not possible?
		handsHigh.insert(std::map<IDType, 
			HandStrength>::value_type(player->GetID(),winnerH[0]));
		
		//insert players lowest card
		if(winnerL.size()>0){
			handsLow.insert(std::map<IDType, 
				HandStrength>::value_type(player->GetID(),winnerL[0]));
		}
	}

	//declare winner high low
	DeclareWinnerHighLow(handsHigh,
		handsLow,
		m_pGameTable,
		m_pPacketSender,
		m_ClientList);
	
	//delay for winner declaration
	WaitForTimerSync(SERVER_DELAY_DECLARE_WINNER);

	if(m_pGameHandler)
		m_pGameHandler->HandlePostGame();
}
