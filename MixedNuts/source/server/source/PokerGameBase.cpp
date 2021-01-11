#include <boost/bind.hpp>
#include "PokerGameBase.h"
#include "DataManager.h"
#include "GameTimeoutDef.h"
#include "GameFactory.h"
#include "PokerHandsLogic.h"

PokerGameBase::PokerGameBase(IDType id): 
GameBase(id),m_pGameHandler(NULL)
{
	
}

PokerGameBase::PokerGameBase(GameIDType id,const GameInfo& gameInfo):
GameBase(id,gameInfo),m_pGameHandler(NULL)
{
	
}

PokerGameBase::~PokerGameBase()
{
}

void 
PokerGameBase::InternalInit()
{
	m_pGameTable.reset( new Table() );
	m_pGameTable->Build( m_pGameInfo->max_players );

	//Set to invalid value to get the next valid delear during begin round
	m_pGameTable->SetDealer(POKER_INVALID_POSITION);

	GameFactory factory;
	factory.CreateLimit( shared_from_this() );

	if(m_pGameInfo){
		m_pPokerGameInfo = 
			boost::static_pointer_cast<GameInfoPoker>(m_pGameInfo);
	}

	//create shuffler
	m_Deck.SetShuffler( new BoostShuffler());
}

void 
PokerGameBase::DealHole(int count,short from)
{
	//deal like a real dealer to minimize repetition 
	//of cards on same position
	DealCardToAll(count,from,eHoleCard,true);

	//send hole cards
	//for(int idx=0;idx<m_pGameInfo->max_players;idx++){
	//	if( m_pGameTable->IsSeatPlaying(idx) ){
	//		//Send notify to client
	//		boost::shared_ptr<ClientSession> client = m_ClientList.GetDataByID(m_pGameTable->GetPlayerAt(idx)->GetID());
	//		m_pPacketSender->SendHoleCard(m_pGameTable->GetPlayerAt(idx)->GetHoleCards(),client);
	//	}
	//}
}

void 
PokerGameBase::DealCardToAll( int count,short from,GamePacket id,bool bSend)
{
	std::map<IDType,CardOwn> list_owned;

	//deal card first
	for(int d=0;d<count;d++){
		int current = from;
		while(true){
			if(m_pGameTable->IsSeatPlaying(current)){
				
				Player* p = m_pGameTable->GetPlayerAt(current);										
				Card card = m_Deck.Pop();
				
				//add cards to hole
				p->AddHoleCard(card);

				//find player
				std::map<IDType, CardOwn>::iterator pos = 
						list_owned.find(p->GetID());	//find by key std::map				
				
				//if not found
				if (pos == list_owned.end()){	
					CardOwn own;
					own.playerID = p->GetID();
					own.hands.push_back(card);
					list_owned.insert(std::map<IDType, 
						CardOwn>::value_type(p->GetID(),own));
				}else{
					CardOwn* pOwner = NULL;
					pOwner = &(pos->second);
					pOwner->hands.push_back(card);
				}				
			}			
			current = (current+1)%m_pGameInfo->max_players;
			if(current == from) break;
		}
	}

	//if dont send then exit
	if(!bSend) return;

	//send new dealt cards
	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( m_pGameTable->IsSeatPlaying(idx) ){
			CardOwn own;
			Player* p = m_pGameTable->GetPlayerAt(idx);
			
			//find player
			std::map<IDType, CardOwn>::const_iterator pos = 
					list_owned.find(p->GetID());	//find by key std::map	
			
			if (pos != list_owned.end()){	
				own = pos->second;
				//Send notify to client
				boost::shared_ptr<ClientSession> client = 
					m_ClientList.GetDataByID(m_pGameTable->GetPlayerAt(idx)->GetID());
				m_pPacketSender->SendCards(id,own.hands,client);
			}					
		}
	}

}

void 
PokerGameBase::StartBettingRound(bool newHand)
{
	//Reset all action to idle
	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( m_pGameTable->IsSeatPlaying(idx) ){
			Player* p = m_pGameTable->GetPlayerAt(idx);
			if(p->GetAction() != eAllIn &&	
				p->GetAction() != eFold)	{
				p->SetAction(eIdle);
				p->SetInRound(true);
			}else if(newHand && p->GetAction() == eAllIn){ 
				//forced bet allin is playing on this round
				p->SetInRound(true);
			}else{
				p->SetInRound(false);
			}			
		}
	}
	
	BettingRound(true,newHand);
}

void 
PokerGameBase::ResetPotRaise()
{
	Pot* pot = m_pGameTable->GetCurrentPot();
	pot->currRaise = 0.0;
	pot->pRaiser = NULL;
}

void 
PokerGameBase::EndRound()
{
	//cancel action timer
	m_gameTimer.cancel();	

	m_pGameTable->CollectBets();
	SendTableSnapShot();
}

void 
PokerGameBase::BettingRound(bool newRound,bool newHand)
{
	unsigned short currIndx = m_pGameTable->GetCurrentBettor();			
	Player* p = m_pGameTable->GetPlayerAt(currIndx);
	if(p==NULL) return;
	
	//send snapshot
	SendTableSnapShot();

	//no more valid players
	//added 03/05/2010
	//update 03/09/2010
	if( !HasValidPlayers() ) {
		Start();
		return;
	}

	if(m_pGameTable->DidPlayersFold())
	{
		EndRound();
		HandleAllFold( );
		return;
	}
	else if(!newHand && m_pGameTable->CheckAllIn(newRound))  		
	{
		//new hand and new round is for catching automatic betting during blinds
		ShowHandAll();
		NextState();
		return;
	}
	else if(m_pGameTable->IsBettingDone())
	{			
		EndRound();
		NextState();
		return;
	}	

	//wait for player action
	boost::shared_ptr<ClientSession> client = m_ClientList.GetDataByID(p->GetID());
	m_pPacketSender->SendWaitAction(client);
	
	//Start timer, from gameInfo timelimit
	// Start
	//disable temporary to test action
	m_gameTimer.expires_from_now(
		boost::posix_time::seconds(m_pGameInfo->action_time_imit));
	m_gameTimer.async_wait(
		boost::bind(&PokerGameBase::TimerExpired, this,				
			boost::asio::placeholders::error));		
}

void
PokerGameBase::TimerExpired(const boost::system::error_code &ec)
{
	unsigned short next_player = 0;
	unsigned short currIndx = m_pGameTable->GetCurrentBettor();			
	Player* p = m_pGameTable->GetPlayerAt(currIndx);
	if(p==NULL) return;	

	//if error code has value then return
	if(ec) return;

	p->SetAction(eFold);
	
	//TODO:Send notification to all about action	
	//m_pPacketSender->SendActionToAll(eFold, p->GetID(),0.0,	m_ClientList);

	//get next bettor
	next_player = m_pGameTable->FindNextValid(currIndx);
	m_pGameTable->SetCurrentBettor(next_player);

	//go back to betting round
	BettingRound();
}

bool PokerGameBase::InternalHandlePacket(boost::shared_ptr<NetPacketRoom> packet,
		boost::shared_ptr<ClientSession> session)
{
	switch(packet->GetCommand()){		
		case eAction:{
			boost::shared_ptr<NetPacketAction> packetAction = 
				boost::dynamic_pointer_cast<NetPacketAction>(packet);
			HandleAction(packetAction,session);
			}break;
		case eShowHands:{
			if(m_pGameTable->GetState() == eShowDownState)
				ShowPlayerHandToAll(session->GetSessionID());
			}break;
		default:
			//notify caller packet not handled
			return false;
	}

	return true;
}

void 
PokerGameBase::ShowPlayerHandToAll(IDType id)
{
	Player* p = m_pGameTable->GetPlayer(id);
	if(!p) return;
	m_pPacketSender->SendHandToAll(p->GetHoleCards(),
		p->GetID(),
		m_ClientList);
}

//TODO: NOTE:
//This is to the assumption that client application handles the value of raise action
void 
PokerGameBase::HandleAction(boost::shared_ptr<NetPacketAction> packet,
		boost::shared_ptr<ClientSession> session)
{
	unsigned short next_player = 0;
	unsigned short currIndx = m_pGameTable->GetCurrentBettor();		
	Player* p = m_pGameTable->GetPlayer(session->GetSessionID());
	Player* p2 = m_pGameTable->GetPlayerAt(currIndx);

	//something wrong happen exit code
	if(p == NULL || p2 == NULL ) return;
	
	//Check if its the right player
	if(p2->GetID() != p->GetID() ) return;	
	
	if( packet->GetAction() != eIdle ) {
		
		float fBet = packet->GetAmount();
		if(packet->GetAction() == eCall){
			fBet += p->GetBet(); 
		}

		//game specific pre-pr0cess
		HandleGameProcess(packet->GetAction(),fBet);

		//check bet validity
		if(!m_pLimit->IsBetValid(fBet,
			packet->GetAction(),p))return;
		
		//set action
		p->SetAction(packet->GetAction());
		if(p->GetAction() == eRaise||
			p->GetAction() == eCall ||
			p->GetAction() == eAllIn) {
			m_pGameTable->PostBet(fBet,p);
		}else if(p->GetAction() == eFold){
			m_pGameTable->RemovePlayerFromPot(p);
			p->SetInRound(false);
		}
	}

	//cancel action timer
	m_gameTimer.cancel();	

	//get next bettor
	next_player = m_pGameTable->FindNextValid(currIndx);
	m_pGameTable->SetCurrentBettor(next_player);

	//go back to betting round
	BettingRound();	
}


int PokerGameBase::GetNumPlayers()
{
	int ctr = 0;
	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( m_pGameTable->IsSeatPlaying(idx) ){
			ctr++;
		}
	}
	return ctr;
}

void 
PokerGameBase::ShowHandAll()
{
	if(m_bShowHands) return;
	
	for(int idx=0;idx<m_pGameInfo->max_players;idx++){
		if( m_pGameTable->IsSeatPlaying(idx) ){
			Player* p = m_pGameTable->GetPlayerAt(idx);
			if(p->GetAction() != eFold) //only show not folded players
				m_pPacketSender->SendHandToAll(p->GetHoleCards(),p->GetID(),m_ClientList);
		}
	}

	m_bShowHands = true;
}

void 
PokerGameBase::SetLimit(boost::shared_ptr<ILimit> limit)
{
	m_pLimit = limit;
}

void PokerGameBase::HandleAllFold()
{
	Player* pWin = NULL;

	for(int idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(player != NULL && 
			player->GetAction() != eFold && 
			player->IsPlaying() ) 
		{
			pWin = player;
			break;
		}
	}

	if(pWin){
		//assumption since all players fold
		//there is only 1 pot available
		Pot* p = m_pGameTable->GetPotAt(0);
		pWin->SetTotalChips(pWin->GetTotalChips()+p->amnt);

		//create temporary handstrength object
		HandStrength hand;
		std::vector<HandStrength> winers;
		hand.SetRanking(HandStrength::NoRank);
		hand.SetPlayerID(pWin->GetID());		
		winers.push_back(hand);

		//notify all about winner
		m_pPacketSender->SendWinnerToAll(winers,0,p->amnt,m_ClientList);
	}
	
	if(m_pGameHandler)
		m_pGameHandler->HandlePostGame();

	//restart game
	Start();
}

void PokerGameBase::NextBettingRound()
{
	ResetPotRaise();
	StartBettingRound();
}

void 
PokerGameBase::SendGameInfo(boost::shared_ptr<ClientSession> session )
{
	m_pPacketSender->SendPokerGameInfo(shared_from_this(),session);
}

void PokerGameBase::NextState()
{
	WaitForTimer(SERVER_DEFAULT_STATE_DELAY);
}

void 
PokerGameBase::SendTableSnapShot()
{
	//TODO: this whole function is TBD
	//Send snapshot to all
	m_pPacketSender->SendAllTablePokerSnapShot(m_pGameTable,m_ClientList);
}

void 
PokerGameBase::DeclareWinner(std::map<IDType,HandStrength>& playerHands)
{
	PokerHandsLogic logic;	
	std::vector<HandStrength> players;
	std::vector<HandStrength> winers;

	//Get winner per pot
	for(unsigned idx=0;idx<m_pGameTable->GetPotSize();idx++)
	{		
		Pot* p = m_pGameTable->GetPotAt(idx);		
		players.clear();
		winers.clear();

		for(unsigned c=0;c<p->playerList.size();c++){
			HandStrength hands;
			std::map<IDType, HandStrength>::const_iterator pos = 
				playerHands.find(p->playerList[c]->GetID());	//find by key std::map
			if (pos != playerHands.end()){
				hands = pos->second;
				players.push_back(hands);
			}
		}

		logic.GetWinningHand(players,winers);		
		float pot_total = p->amnt/winers.size();
			
		//award the winner
		for(unsigned w=0;w<winers.size();w++){
			Player* player = m_pGameTable->GetPlayer(winers[w].GetPlayerID());
			if(!player) continue;
			player->SetTotalChips(player->GetTotalChips()+pot_total);

			//send hand for evidence
			m_pPacketSender->SendHandToAll(player->GetHoleCards(),
				player->GetID(),m_ClientList);
		}

		m_pPacketSender->SendWinnerToAll(winers,idx,pot_total,m_ClientList);		
	}

	//delay for winner declaration
	WaitForTimerSync(SERVER_DELAY_DECLARE_WINNER);	

	if(m_pGameHandler)
		m_pGameHandler->HandlePostGame();
}

void 
PokerGameBase::SetGameHandler(IGameHandler* handler)
{
	m_pGameHandler = handler;
}

void PokerGameBase::HandleWin()
{
	//set state
	m_pGameTable->SetState(eShowDownState);
}