#include "StudHiLo.h"
#include "HighLowBase.h"
#include "GameTimeoutDef.h"

//StudHiLo::StudHiLo(void)
//{
//}
StudHiLo::StudHiLo(GameIDType id):
	SevenCardStud(id)
{
	//The difference in Hi-Lo is 
	//that the bets cannot be doubled on the fourth street - LowBall
	m_bProtectPair = false;
}

StudHiLo::~StudHiLo(void)
{
}

void StudHiLo::ShowDown()
{
	PokerHandsLogic logic;
	std::map<IDType,HandStrength> highHands;
	std::map<IDType,HandStrength> lowHands;

	int idx=0;

	//create player hands for remaining players
	for(idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player || player->GetAction() == eFold || 
			!player->IsPlaying()) continue;
		
		HandStrength hand = logic.CreateHandStrength(player->GetHoleCards(),
			player->GetID());
		highHands.insert(std::map<IDType, HandStrength>::value_type(player->GetID(),hand));

		std::vector<Card> low_cards;		
		if(logic.IsHandQualifyLow(player->GetHoleCards(),low_cards))
		{
			HandStrength hand;
			hand.Cards = low_cards;
			hand.SetPlayerID(player->GetID());
			hand.SetRanking(HandStrength::LowHand);
			lowHands.insert(std::map<IDType, HandStrength>::value_type(player->GetID(),hand));
		}				
	}

	//declare winner high low
	DeclareWinnerHighLow(highHands,
		lowHands,
		m_pGameTable,
		m_pPacketSender,
		m_ClientList);

	//delay for winner declaration
	WaitForTimerSync(SERVER_DELAY_DECLARE_WINNER);
	
	if(m_pGameHandler)
		m_pGameHandler->HandlePostGame();
}

