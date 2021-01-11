#include <map>

#include "TexasHoldem.h"
#include "Shuffler.h"
#include "GameTimeoutDef.h"
#include "PokerHandsLogic.h"

//TexasHoldem::TexasHoldem(void)
//{
//}

TexasHoldem::~TexasHoldem(void)
{
}

TexasHoldem::TexasHoldem(GameIDType id): 
	PokerGameBase(id),
	m_pfnNextState(NULL)
{
	
}

TexasHoldem::TexasHoldem(GameIDType id,const GameInfo& gameInfo):
	PokerGameBase(id,gameInfo),
	m_pfnNextState(NULL)
{

}

//void TexasHoldem::InternalInit()
//{
//	//m_pGameTable.reset( new TableHoldem() );
//	//m_pGameTable->Build( m_pGameInfo->max_players );
//	//m_pTable = boost::static_pointer_cast<TableHoldem>(m_pGameTable);
//
//	////Set to invalid value to get the next valid delear during begin round
//	//m_pTable->SetDealer(POKER_INVALID_POSITION);
//	//m_pTable->SetSBIndex(POKER_INVALID_POSITION);
//	//m_pTable->SetBBIndex(POKER_INVALID_POSITION);
//
//	/*PokerGameBase::InternalInit();*/
//}

void TexasHoldem::Start()
{
	m_pGameTable->SetState(eWaitingPlayers);
	m_pfnNextState = &TexasHoldem::PlayNextRound;
	SendTableSnapShot();	
	WaitForTimer(SERVER_GAME_START_ROUND);
}

void  
TexasHoldem::PlayNextRound()
{
	if(GameBase::HasRoundPlayers()){
		BeginRound();
		m_pfnNextState = &TexasHoldem::InternalDealHole;
		WaitForTimer(SERVER_GAME_DEAL_HOLE);	

	}else{
		Start();
	}
}

void 
TexasHoldem::BeginRound()
{
	//Pot pot;
	short SBIdx, BBIdx;
	
	//set show hand variable
	m_bShowHands = false;

	//reset table
	m_pGameTable->ResetTable();
	m_pGameTable->SetState(eNewRound);

	//reset Deck
	m_Deck.Reset();

	//set dealer and big blind
	//if( !m_pTable->IsSeatPlaying(m_pTable->GetDealer()) )
	m_pGameTable->SetDealer(m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));
	
	SBIdx = m_pGameTable->FindNextValid(m_pGameTable->GetDealer());
	BBIdx = m_pGameTable->FindNextValid(SBIdx);
	/*m_pTable->SetSBIndex(m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));
	m_pTable->SetBBIndex(m_pGameTable->FindNextValid(m_pTable->GetSBIndex()));
	SBIdx = m_pTable->GetSBIndex();
	BBIdx = m_pTable->GetBBIndex();*/

	//get initial bets for blinds	
	float fStake = m_pGameInfo->stake;
	m_pGameTable->PostBet(fStake/2.0f,m_pGameTable->GetPlayerAt(SBIdx));
	m_pGameTable->PostBet(fStake,m_pGameTable->GetPlayerAt(BBIdx));
	m_pGameTable->SetCurrentBettor(m_pGameTable->FindNextValid(BBIdx));		

	//clear community cards
	m_CommunityCards.clear();

	//reset limit variables
	m_pLimit->Update();

	//send initial snap shot
	SendTableSnapShot();
}

void 
TexasHoldem::InternalDealHole()
{
	short SBIdx = m_pGameTable->FindNextValid(m_pGameTable->GetDealer());
	//deal 2 cards as hole from SB position
	DealHole(2,SBIdx);
	
	m_iHandCount++;
	m_pfnNextState = &TexasHoldem::DealFlop;	
	m_pGameTable->SetState(eHoleState);
	
	//reset limit variables
	m_pLimit->Update();

	StartBettingRound(true);
}

void 
TexasHoldem::DealFlop()
{
	//WaitForTimer(SERVER_GAME_DEAL_FLOP);
	m_Deck.Pop();						//Burn card
	
	for(int x=0;x<3;x++)
		m_CommunityCards.push_back(m_Deck.Pop());	

	//Notify all players
	m_pPacketSender->SendFlop(m_CommunityCards,m_ClientList);

	//reset limit variables
	m_pLimit->Update();

	m_pfnNextState = &TexasHoldem::DealTurn;
	m_pGameTable->SetState(eFlopState);
	NextBettingRound();
}

void TexasHoldem::DealTurn()
{
	m_Deck.Pop();						//Burn card
	Card card = m_Deck.Pop();	
	m_CommunityCards.push_back(card);	

	//Notify all players
	m_pPacketSender->SendCard(eTurn,card,m_ClientList);
	
	//reset limit variables
	m_pLimit->Update();

	m_pfnNextState = &TexasHoldem::DealRiver;
	m_pGameTable->SetState(eTurnState);
	NextBettingRound();
}

void TexasHoldem::DealRiver()
{
	m_Deck.Pop();						//Burn card
	Card card = m_Deck.Pop();	
	m_CommunityCards.push_back(card);
	
	//Notify all players
	m_pPacketSender->SendCard(eRiver,card,m_ClientList);
	
	//reset limit variables
	m_pLimit->Update();

	m_pfnNextState = &TexasHoldem::HandleWin;
	m_pGameTable->SetState(eRiverState);
	NextBettingRound();
}

void 
TexasHoldem::NextBettingRound()
{
	//Set current bettor as the one next to dealder
	m_pGameTable->SetCurrentBettor(m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));
	PokerGameBase::NextBettingRound();
}

void 
TexasHoldem::ShowDown()
{
	PokerHandsLogic logic;
	std::map<IDType,HandStrength> playerHands;
	int idx=0;

	//create player hands for remaining players
	for(idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player || player->GetAction() == eFold || 
			!player->IsPlaying()) continue;
		
		HandStrength hands =  logic.CreateHandStrength(m_CommunityCards,
			player->GetHoleCards(),player->GetID());
		playerHands.insert(std::map<IDType, HandStrength>::value_type(player->GetID(),hands));
	}

	DeclareWinner(playerHands);
}


void 
TexasHoldem::HandleWin()
{
	//call base
	PokerGameBase::HandleWin();

	//get winner
	ShowDown();	

	//start next round, after delay
	m_pfnNextState = &TexasHoldem::Start;
	WaitForTimer(SERVER_DELAY_SHOWDOWN_AFTERMATH);	
}

//void 
//TexasHoldem::SendTableSnapShot()
//{
//	//Send snapshot to all
//	m_pPacketSender->SendAllTableHoldemSnapShot(m_pTable,m_ClientList);
//}

void TexasHoldem::TimerWakeUp()
{
	(this->*m_pfnNextState)();
}

//void TexasHoldem::NextState()
//{
//	WaitForTimer(SERVER_DEFAULT_STATE_DELAY);
//}

void 
TexasHoldem::AddSession(boost::shared_ptr<ClientSession> session)
{
	GameBase::AddSession(session);

	//send community cards the snap shot to client
	//Added March 5, 2010, fix for bug found that new entry player cannot see
	//the whole community cards
	m_pPacketSender->SendCards(eCommunityCard,m_CommunityCards,session);
}

void 
TexasHoldem::HandleGameProcess(PokerAction action, float fBet)
{
	/*None*/
}
