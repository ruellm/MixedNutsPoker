#include "SevenCardStud.h"
#include "GameTimeoutDef.h"
#include "Card.h"
#include "PokerHandsLogic.h"

//SevenCardStud::SevenCardStud(void)
//{
//}

SevenCardStud::SevenCardStud(GameIDType id): 
	PokerGameBase(id),
	m_pfnNextState(NULL),
	m_bProtectPair(true)
{
	
}


SevenCardStud::~SevenCardStud(void)
{
}

void 
SevenCardStud::InternalInit()
{	
	//call init from base
	PokerGameBase::InternalInit();
	
	//protect pair only works on fixed limit	
	if(m_pPokerGameInfo->limit_type != eFixedLimit)
		m_bProtectPair = false; 
}

void 
SevenCardStud::Start()
{
	m_pGameTable->SetState(eWaitingPlayers);
	m_pfnNextState = &SevenCardStud::PlayNextRound;
	SendTableSnapShot();	
	WaitForTimer(SERVER_GAME_START_ROUND);
}

void 
SevenCardStud::BeginRound() 
{
	//set show hand variable
	m_bShowHands = false;

	//reset table
	m_pGameTable->ResetTable();
	m_pGameTable->SetState(eNewRound);

	//reset Deck
	m_Deck.Reset();

	//set dealer and big blind	
	m_pGameTable->SetDealer(m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));

	//Get all antes from players
	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( !m_pGameTable->IsSeatPlaying(idx) ) continue;
		Player* p = m_pGameTable->GetPlayerAt(idx);
		m_pGameTable->PostBet(m_pPokerGameInfo->fAntes,p);
		p->SetAction(eAntes);
	}

	//reset limit variables
	m_pLimit->Update();

	//send initial snap shot
	SendTableSnapShot();	
}

void 
SevenCardStud::PlayNextRound()
{
	if(GameBase::HasRoundPlayers()){
		BeginRound();
		m_pfnNextState = &SevenCardStud::InternalDealHole;
		WaitForTimer(SERVER_GAME_DEAL_HOLE);	

	}else{
		Start();
	}
}

void 
SevenCardStud::InternalDealHole()
{
	//reset pot after antes
	m_pGameTable->CollectBets();
	ResetPotRaise();

	DealHole(3,m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));
	
	m_iHandCount++;
	m_pGameTable->SetState(eHoleState);
	
	//reset limit variables
	m_pLimit->Update();	

	//send open cards to all
	SendOpenCard(e3rdStreet);

	//assign current bettor to player holding the lowest
	//open card
	m_pGameTable->SetCurrentBettor(GetBringInSeat());
	
	//set next state
	m_pfnNextState = &SevenCardStud::FourthStreet;		

	//do "bring in" betting
	//client side: 
	//if state = hole and currRaise == 0 display bring in or raise
	//if bring in clicked, it it considered as raise
	//TODO: check raise checking
	StartBettingRound(true);
}


void SevenCardStud::FourthStreet()
{	
	DealCardToAll(1, m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));		
	
	m_pGameTable->SetState(e4thStreetState);
	SendOpenCard(e4rthStreet);
	m_pGameTable->SetCurrentBettor(GetDealerSeat());
	
	//Set parameter before start of betting round
	if( m_bProtectPair ){
		m_b4thStreetRaise = IsTherePairOnBoard();
		m_pPacketSender->SendAllRoomParam(e4thStreetParam,(int)m_b4thStreetRaise,m_ClientList);
		WaitForTimerSync(SERVER_DELAY_PACKET);		
	}

	//reset limit variables
	m_pLimit->Update();

	//set next state
	m_pfnNextState = &SevenCardStud::FifthStreet;	
	NextBettingRound();
}


void SevenCardStud::FifthStreet()
{
	DealCardToAll(1, m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));		
	//set state
	m_pGameTable->SetState(e5thStreetState);

	//Set parameter before start of betting round
	//disable on 5th street
	if( m_bProtectPair ){
		m_b4thStreetRaise = false;
		m_pPacketSender->SendAllRoomParam(e4thStreetParam,
			(int)m_b4thStreetRaise,m_ClientList);
		WaitForTimerSync(SERVER_DELAY_PACKET);	
	}
	
	SendOpenCard(e5thStreet);
	m_pGameTable->SetCurrentBettor(GetDealerSeat());

	//reset limit variables
	m_pLimit->Update();

	//set next state
	m_pfnNextState = &SevenCardStud::SixthStreet;	
	NextBettingRound();
}


void SevenCardStud::SixthStreet()
{
	DealCardToAll(1,m_pGameTable->FindNextValid(m_pGameTable->GetDealer()));	
	
	//set state
	m_pGameTable->SetState(e6thStreetState);
	SendOpenCard(e6thStreet);
	m_pGameTable->SetCurrentBettor(GetDealerSeat());

	//reset limit variables
	m_pLimit->Update();

	//set next state
	m_pfnNextState = &SevenCardStud::SeventhStreet;	
	NextBettingRound();
}

void SevenCardStud::SeventhStreet()
{
	DealCardToAll(1, m_pGameTable->FindNextValid(m_pGameTable->GetDealer()),
		e7thStreet,true);

	//just notify clients that 7th street was dealt
	//m_pPacketSender->SendAllRoomParam(e7hStreet,0,m_ClientList);   

	//set state
	m_pGameTable->SetState(e7thStreetState);
	m_pGameTable->SetCurrentBettor(GetDealerSeat());

	//reset limit variables
	m_pLimit->Update();

	//set next state
	m_pfnNextState = &SevenCardStud::HandleWin;	
	NextBettingRound();
}

void 
SevenCardStud::HandleGameProcess(PokerAction action, float fBet)
{
	//handle process
	if(m_pGameTable->GetState() != e4thStreetState) return;
	if(m_b4thStreetRaise != true) return;
	if( !m_bProtectPair ) return;
	if(m_pPokerGameInfo->limit_type != eFixedLimit) return;

	//compute current raise
	Pot* pot = m_pGameTable->GetCurrentPot();
	if(pot->currRaise == 0){
		if( fBet == m_pGameInfo->stake ){
			m_b4thStreetRaise = false;
		}
	}else{
		if( (pot->currRaise +m_pGameInfo->stake) ==fBet)
		m_b4thStreetRaise = false;
	}

	//try to fool server
	//TODO: watch out for this this might affect client
	if(m_b4thStreetRaise == false){
		m_pGameTable->SetState(e5thStreetState);
	}

	//Set parameter before start of betting round
	m_pPacketSender->SendAllRoomParam(e4thStreetParam,m_b4thStreetRaise,m_ClientList);
	WaitForTimerSync(SERVER_DELAY_PACKET);
}

void SevenCardStud::TimerWakeUp()
{
	(this->*m_pfnNextState)();
}

int SevenCardStud::GetBringInSeat()
{
	//this function is only used during "bring-in"
	int index = 0;
	const int thirdStreet = 2;

	Card smallest;

	smallest.SetFace(Card::Joker);
	smallest.SetSuite(Card::None);

	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( !m_pGameTable->IsSeatPlaying(idx) ) continue;
		Player* p = m_pGameTable->GetPlayerAt(idx);
		
		if( p->GetAction() == eFold || 
			p->GetAction() == eAllIn) continue;

		Card c = p->GetHoleCards()[thirdStreet];
		if( c < smallest ){
			smallest = c;
			index = idx;
		}
		else if( c == smallest)
		{
			//compare by suite
			if(c.GetSuite() < smallest.GetSuite()){
				smallest = c;
				index = idx;
			}
		}
	}
	return index;
}

bool 
SevenCardStud::IsTherePairOnBoard()
{
	bool flag = false;
	PokerHandsLogic logic;
	std::vector<CardOwn> ownerList = GetOpenCards();	
	
	//generate hand strength for all users
	for(unsigned c=0;c<ownerList.size();c++){
		CardOwn owner = ownerList[c];
		HandStrength::HandRanking rank = logic.GetHandRanking(owner.hands);
		
		if(rank == HandStrength::OnePair ) {
			flag = true;
			break;
		}
	}

	return flag;
}

int 
SevenCardStud::GetDealerSeat()
{
	PokerHandsLogic logic;
	HandStrength extreme_win;
	std::vector<HandStrength> players;
	std::vector<HandStrength> winers;
	std::vector<CardOwn> ownerList = GetOpenCards();	

	//generate hand strength for all users
	for(unsigned c=0;c<ownerList.size();c++){
		CardOwn owner = ownerList[c];
		HandStrength hands =  logic.CreateHandStrength(owner.hands,
			owner.playerID);
		players.push_back(hands);
	}

	//get winning hand
	logic.GetWinningHand(players,winers);
	extreme_win = winers[0];

	//get the only winner using the suite
	for(unsigned z=0;z<winers.size();z++){
		HandStrength current = winers[z];	
		if( current.Cards[0].GetSuite() > 
			extreme_win.Cards[0].GetSuite() ) {
			extreme_win = current;			
		}
	}
	
	//get playerID
	Player* p = 
		m_pGameTable->GetPlayer(extreme_win.GetPlayerID());
	if( p == NULL ) return 0;

	return p->GetSeatNum();
}

void 
SevenCardStud::SendAllOpenCards()
{
	std::vector<CardOwn> ownerList = GetOpenCards();
	if( ownerList.size() <= 0 ) return;	
	//send open cards
	m_pPacketSender->SendOpenCard(ownerList,eOpenCards,m_ClientList);
}

void 
SevenCardStud::SendOpenCard(GamePacket id)
{
	std::vector<CardOwn> ownerList;
	const int open_start_idx = 2;

	//get open index
	int index = 0;
	if(m_pGameTable->GetState() == eHoleState)
		index = 0;
	else if(m_pGameTable->GetState() == e4thStreetState)
		index = 1;
	else if(m_pGameTable->GetState() == e5thStreetState)
		index = 2;
	else if(m_pGameTable->GetState() == e6thStreetState)
		index = 3;

	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( !m_pGameTable->IsSeatPlaying(idx) ) continue;
		Player* p = m_pGameTable->GetPlayerAt(idx);
		
		if( p->GetAction() == eFold || 
			p->GetAction() == eAllIn) continue;

		CardOwn owner;
		owner.playerID = p->GetID();
		
		//populate player cards
		std::vector<Card> hole = p->GetHoleCards();
		owner.hands.push_back(hole[open_start_idx+index]);

		//add to list
		ownerList.push_back(owner);
	}
	
	m_pPacketSender->SendOpenCard(ownerList,id,m_ClientList);
}

std::vector<CardOwn> 
SevenCardStud::GetOpenCards()
{
	std::vector<CardOwn> ownerList;
	const int open_start_idx = 2;
	int count = GetOpenCardsCount();

	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( !m_pGameTable->IsSeatPlaying(idx) ) continue;
		Player* p = m_pGameTable->GetPlayerAt(idx);
		
		if( p->GetAction() == eFold || 
			p->GetAction() == eAllIn) continue;

		CardOwn owner;
		owner.playerID = p->GetID();
		
		//populate player cards
		std::vector<Card> hole = p->GetHoleCards();
		for(int z = 0;z<count;z++){
			owner.hands.push_back(hole[open_start_idx+z]);
		}

		//add to list
		ownerList.push_back(owner);
	}

	return ownerList;
}
int 
SevenCardStud::GetOpenCardsCount()
{
	int count = 0;
	if(m_pGameTable->GetState() == eHoleState)
		count = 1;
	else if(m_pGameTable->GetState() == e4thStreetState)
		count = 2;
	else if(m_pGameTable->GetState() == e5thStreetState)
		count = 3;
	else if(m_pGameTable->GetState() == e6thStreetState ||
		m_pGameTable->GetState() == e7thStreetState)
		count = 4;

	return count;
}

void 
SevenCardStud::AddSession(boost::shared_ptr<ClientSession> session)
{
	GameBase::AddSession(session);
	SendAllOpenCards();
}


void SevenCardStud::ShowDown()
{
	PokerHandsLogic logic;
	std::map<IDType,HandStrength> playerHands;
	int idx=0;

	//create player hands for remaining players
	for(idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player || player->GetAction() == eFold || 
			!player->IsPlaying()) continue;
		
		HandStrength hands =  logic.CreateHandStrength(player->GetHoleCards(),player->GetID());
		playerHands.insert(std::map<IDType, HandStrength>::value_type(player->GetID(),hands));
	}

	DeclareWinner(playerHands);
}

void 
SevenCardStud::HandleWin()
{
	//call base
	PokerGameBase::HandleWin();

	//get winner
	ShowDown();	

	//start next round, after delay
	m_pfnNextState = &SevenCardStud::Start;
	WaitForTimer(SERVER_DELAY_SHOWDOWN_AFTERMATH);	
}

//void SevenCardStud::SendGameInfo(boost::shared_ptr<ClientSession> session )
//{
//	m_pPacketSender->SendSevenCardStudGameInfo(shared_from_this(),session);
//}