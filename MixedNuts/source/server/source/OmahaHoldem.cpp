#include "OmahaHoldem.h"
#include "PokerHandsLogic.h"

//OmahaHoldem::OmahaHoldem(void)
//{
//}
OmahaHoldem::OmahaHoldem(GameIDType id): 
	TexasHoldem(id)
{
	
}

OmahaHoldem::~OmahaHoldem(void)
{
}

void OmahaHoldem::InternalDealHole()
{
	short SBIdx = 
		m_pGameTable->FindNextValid(m_pGameTable->GetDealer());
	
	//deal 4 cards as hole from SB position
	DealHole(4,SBIdx);
	
	m_iHandCount++;
	m_pfnNextState = &TexasHoldem::DealFlop;	
	m_pGameTable->SetState(eHoleState);
	
	//reset limit variables
	m_pLimit->Update();

	StartBettingRound(true);
}

void OmahaHoldem::ShowDown()
{
	PokerHandsLogic logic;
	std::map<IDType,HandStrength> playerHands;

	//community card group per 3
	std::vector< std::vector<Card> > community_sets = 
		GroupCardByThree(m_CommunityCards);
	
	for(int idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player || player->GetAction() == eFold || 
			!player->IsPlaying()) continue;

		//hand list of player
		std::vector<HandStrength> handList;
		std::vector<HandStrength> winner;

		//generate player set cards
		std::vector< std::vector<Card> > player_sets = 
			GroupCardByTwo(player->GetHoleCards());

		//generate hand strength from community sets and player set
		for(unsigned ps=0;ps<player_sets.size();ps++){
			for(unsigned cs=0;cs<community_sets.size();cs++){
				HandStrength hands =  logic.CreateHandStrength(community_sets[cs],
					player_sets[ps],player->GetID());
				handList.push_back(hands);
			}
		}

		//generate best hand for player
		logic.GetWinningHand(handList,winner);
		//insert the winning hand for the player
		//TODO: tied is not possible?
		playerHands.insert(std::map<IDType, 
			HandStrength>::value_type(player->GetID(),winner[0]));
	}

	//declare winner
	DeclareWinner(playerHands);
}
