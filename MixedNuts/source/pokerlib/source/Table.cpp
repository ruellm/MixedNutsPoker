#include "Table.h"

void Table::Build(int numSeats)
{
	unsigned idx = 0;
	m_iDealerIndex = 0;
	m_usCurrentBettor = 0;

	for(idx=0;idx<numSeats;idx++){
		Seat seat;
		seat.occupied = false;
		//seat.seat_number  = idx;
		seat.player = NULL;	
		m_Seats.push_back(seat);
	}

	//set initla values
	ResetTable();
}

Player*
Table::GetPlayerAt(short seatno)
{
	if( seatno < 0 || seatno >= m_Seats.size() ) return NULL;
	if( m_Seats[seatno].player == NULL && m_Seats[seatno].occupied == false) return NULL;
	
	return m_Seats[seatno].player;
}

Player* 
Table::GetPlayer(IDType id)
{
	for(unsigned idx = 0;idx<m_Seats.size();idx++) {
		if(m_Seats[idx].player != NULL && 
			m_Seats[idx].player->GetID() == id)
			return m_Seats[idx].player;
	}
	return NULL;
}

void 
Table::RemovePlayer(short seatno)
{
	if( seatno < 0 || seatno >= m_Seats.size()) return;
	if( m_Seats[seatno].player == NULL && m_Seats[seatno].occupied == false) return;
	
	if(m_Seats[seatno].player != NULL ) {
		delete m_Seats[seatno].player;
		m_Seats[seatno].player = NULL;
	}
}

bool 
Table::IsSeatVacant(short seatno)
{
	if( seatno < 0 || seatno >= m_Seats.size()) return false;
	return !(m_Seats[seatno].occupied);
}
void 
Table::SeatDownPlayer(short seatno, Player* player)
{
	if( seatno < 0 || seatno >= m_Seats.size()) return;
	if( m_Seats[seatno].occupied == true) return;

	m_Seats[seatno].player = player;
	m_Seats[seatno].occupied = true;
	player->SetSeatNum(seatno);
}

int 
Table::GetDealer()const
{	return m_iDealerIndex; }

void 
Table::SetDealer(short idx)
{	m_iDealerIndex = idx; }

bool 
Table::IsSeatPlaying(short seatno)
{	
	if( !IfSetValid(seatno)) return false;
	return m_Seats[seatno].player->IsPlaying();	
}

bool Table::IfSetValid(short seatno)
{
	if( seatno < 0 || seatno >= m_Seats.size()) return false;
	if( m_Seats[seatno].player == NULL || m_Seats[seatno].occupied == false) return false;
	return true;
}
bool Table::IfSeatIsPlayingRound(short seatno)
{
	if( !IfSetValid(seatno) ) return false;	
	Player* p = GetPlayerAt(seatno);
	if(!p->IsPlaying()) return false;
	if(p->GetAction() == eAllIn || p->GetAction() == eFold) return false;
	
	return true;	
}

short 
Table::FindNextValid(short seatno)
{	
	int nxt = (seatno+1)%m_Seats.size();
	while(true){
		if(IfSeatIsPlayingRound(nxt)) return nxt;
		nxt = (nxt+1)%m_Seats.size();
		if(nxt == seatno) break;
	}
	
	return seatno;
}

short 
Table::GetSeatCount() const
{
	return m_Seats.size();
}

short 
Table::GetCurrentBettor() const
{
	return m_usCurrentBettor;
}

void Table::SetCurrentBettor(unsigned short idx)
{
	m_usCurrentBettor = idx;
}

short 
Table::GetRoundPlayerCount()
{
	short counter = 0;
	for(int idx = 0;idx<m_Seats.size();idx++) {		
		if(IsSeatPlaying(idx)){
			counter++;
		}
	}
	return counter;
}

bool Table::DidEveryOneCheck()
{
	short counter = CountIsRoundPlayers();
	int ctr=0;
	for(int idx=0;idx<m_Seats.size();idx++){
		if( !IsSeatPlaying(idx)) continue;
		Player* p = GetPlayerAt(idx);
		if( p->IsInRound() && p->GetAction() == eCheck){
			ctr++;
		}	
	}
	
	if(ctr==counter)
		return true;	

	return false;
}

bool 
Table::IsBettingDone()
{	
	Pot* currpot = GetCurrentPot();
	
	//if all players checked
	if(DidEveryOneCheck()) return true;		

	//compute if allbets are equal
	int ctr=0;
	short counter = CountIsRoundPlayers();	

	//someone raise, check if all called
	if( currpot->currRaise != 0.0f && 
		currpot->pRaiser != NULL) {				
		for(int idx=0;idx<m_Seats.size();idx++){
			if( !IsSeatPlaying(idx)) continue;

			Player* p = GetPlayerAt(idx);
			if( !p->IsInRound() ) continue;

			if(p->GetAction() == eFold || 
				p->GetAction() == eIdle) continue;
			
			if( p->GetBet() == currpot->currRaise ){
				ctr++;
			}else if(p->GetAction()==eAllIn){
				ctr++;
			}
		}		
	}

	if( ctr == counter )
		return true;
	
	//no one is contesting current pot
	if(currpot->playerList.size() <=1)
		return true;
	return false;
}

short 
Table::CountIsRoundPlayers()
{
	//count in round players
	short counter = 0;
	for(int idx = 0;idx<m_Seats.size();idx++) {		
		if(IsSeatPlaying(idx) &&
			GetPlayerAt(idx)->IsInRound() &&
			GetPlayerAt(idx)->GetAction() != eFold){
			counter++;
		}
	}
	return counter;
}

bool Table::CheckAllIn(bool newRound)
{
	short ctr = 0;
	short counter = CountIsRoundPlayers();

	for(int idx=0;idx<m_Seats.size();idx++){
		if( !IsSeatPlaying(idx)) continue;
		Player* p = GetPlayerAt(idx);
		if( p->IsInRound() && p->GetAction() == eAllIn){
			ctr++;
		}	
	}		

	if( (ctr == counter-1 || ctr == counter) && newRound)
		return true;

	//there is only 1 active player, 
	//either folded or allin in previous round
	if(counter == 1)
		return true;
	
	return false;
}

bool 
Table::DidPlayersFold()
{
	//check if all players fold except for 1
	short ctr = 0;
	for(int idx=0;idx<m_Seats.size();idx++){
		if( IsSeatPlaying(idx) && 
			GetPlayerAt(idx)->GetAction() == eFold){
			ctr++;
		}
	}
	////Pot* currpot = GetCurrentPot();	
	//if( ctr == GetRoundPlayerCount()-1 ) 
	//	return true;

	short counter = 0;
	for(int idx = 0;idx<m_Seats.size();idx++) {		
		if(IsSeatPlaying(idx)){
			counter++;
		}
	}

	if( ctr == counter-1 ) 
		return true;

	return false;
}

void Table::PostBet( float amnt, Player* p)
{
	Pot* currpot = GetCurrentPot();
	
	if(amnt >= p->GetTotalChips()){
		amnt = p->GetTotalChips();
		p->SetAction(eAllIn);
	}

	p->SetBet(amnt);
	if(currpot->currRaise < amnt ){
		currpot->currRaise = amnt;
		currpot->pRaiser = p;
	}
	
}

Pot* Table::GetCurrentPot()
{
	return &(m_PotList.at(m_PotList.size()-1));
}

void 
Table::ResetPlayers()
{
	//Reset each player action
	for(int idx=0;idx<m_Seats.size();idx++)
	{
		if( !IfSetValid(idx)) continue;
		Player* p = GetPlayerAt(idx);
		p->ResetHoleCards();
		p->SetAction(eIdle);
		p->SetBet(0.0);	
		p->SetIsPlaying(p->GetTotalChips() > 0.0);
	}
}


void 
Table::CollectBets()
{
	do{
		//get smallest bet
		float smallest_bet=0.0;
		bool need_sidepot = false;
		for(int idx=0;idx<m_Seats.size();idx++){			

			if(!IfSetValid(idx)) continue;
			Player* p = GetPlayerAt(idx);
			if(!p->IsPlaying())continue;
			if( p->GetBet() == 0.0 ) continue;
			if( smallest_bet == 0.0 ) {
				smallest_bet = p->GetBet();
			}else if( p->GetBet() < smallest_bet ) {
				smallest_bet = p->GetBet();
				need_sidepot = true;
			}else if(p->GetBet() > smallest_bet) {
				need_sidepot = true;
			}			
		}

		// no bets all checked
		if( smallest_bet == 0.0 ) return;
		
		// last pot is current pot
		Pot* curpot = GetCurrentPot();
		if( curpot->final ) {
			AddNewPot();
			curpot = GetCurrentPot();
			
			//no one is contesting this pot, time to end
			if(curpot->playerList.size() <= 1){	

				//TODO: give back extra to player
				Player* player_cash = curpot->playerList[0];
				player_cash->SetBet(player_cash->GetBet()-curpot->amnt);

				//delete this pot
				std::vector<Pot>::iterator it = m_PotList.begin();
				for(;it != m_PotList.end();it++){
					if( &(*it) == curpot ){
						m_PotList.erase(it);
						break;
					}
				}
				return;
			}
		}	

		for(int idx=0;idx<m_Seats.size();idx++){			
			if( !IsSeatPlaying(idx)) continue;
			
			Player* p = GetPlayerAt(idx);		
			if( p->GetBet() == 0.0 ) continue;
			
			//Get bet of folded players
			if( p->GetAction() == eFold ) {
				curpot->amnt += p->GetBet();
				p->SetTotalChips( p->GetTotalChips() - p->GetBet());
				p->SetBet(0.0);
				continue;
			}

			//collect the bet into the pot
			if(!need_sidepot) {
				curpot->amnt += p->GetBet();
				p->SetTotalChips( p->GetTotalChips() - p->GetBet());
				p->SetBet(0.0);
			}else{
				curpot->amnt += smallest_bet;
				p->SetTotalChips( p->GetTotalChips() - smallest_bet);
				p->SetBet(p->GetBet()-smallest_bet);
			}

			//marked pot final if a player is allin
			if(p->GetTotalChips() == 0.0 && p->GetAction() != eFold){
				p->SetAction(eAllIn);
				curpot->final = true;
			}
				
		}

		if( !need_sidepot)
			break;

	}while(true);
}

void Table::AddNewPot()
{
	Pot pot;
	pot.amnt = 0.0;
	pot.pRaiser = NULL;
	pot.currRaise = 0.0;
	pot.final = false;

	//Populate players playing
	for(int idx=0;idx<m_Seats.size();idx++){			
		if( !IsSeatPlaying(idx)) continue;	
		Player* p = GetPlayerAt(idx);	
		if( p->GetTotalChips() > 0.0 && 
			p->GetAction() != eFold) {
			pot.playerList.push_back(p);
		}
	}


	m_PotList.push_back(pot);
}

void 
Table::RemovePlayerFromPot(Player* p,Pot* pot)
{
	if(pot==NULL){
		//If argument is null, use current pot
		pot = GetCurrentPot();
	}

	std::vector<Player*>::iterator it = pot->playerList.begin();
	for(;it != pot->playerList.end();it++){
		if( (*it) == p ){
			pot->playerList.erase(it);
			return;
		}
	}
}
void 
Table::RemovePlayerFromAllPot(Player* p)
{
	for(int x=0;x<m_PotList.size();x++){
		RemovePlayerFromPot(p,&m_PotList[x]);
	}
}
void 
Table::ResetTable()
{	
	ResetPlayers();
	m_PotList.clear();
	AddNewPot();	
}

void 
Table::SetState(TableState state)
{
	m_State = state;
}

TableState 
Table::GetState() const
{
	return m_State;
}

void 
Table::StandUpPlayer(IDType id)
{
	for(int idx=0;idx<m_Seats.size();idx++){			
		if( !IfSetValid(idx)) continue;	
		Player* p = GetPlayerAt(idx);	
		if( p->GetID() == id) {
			m_Seats[idx].occupied = false;
			m_Seats[idx].player = NULL;
			
			//TODO: TBD maybe not necessary
			p->SetAction(eFold);

			RemovePlayerFromAllPot(p);
			delete p;
			return;
		}
	}
}

size_t 
Table::GetPotSize() const
{
	return m_PotList.size();
}

Pot* 
Table::GetPotAt(int ix)
{
	if( ix >= m_PotList.size()|| ix<0) return NULL;
	return &m_PotList[ix];
}