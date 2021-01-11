#include <boost/bind.hpp>
#include "SitAndGo.h"
#include "IOServicePool.h"
#include "GameTimeoutDef.h"

SitAndGo::SitAndGo(GameIDType id):
	PokerGameBase(id),
	m_pfnNextState(NULL),
	m_BlindsTimer(IOServicePool::GetInstance()->GetIOServices()),
	m_nLevel(0)
{}

void SitAndGo::Start()
{
	boost::shared_ptr<Table> pTable = m_pGame->GetGameTable();
	pTable->SetState(eWaitingPlayers);
	m_pfnNextState = &SitAndGo::PlayNextRound;
	//SendTableSnapShot();	
	m_pGame->SendTableSnapShot();
	WaitForTimer(SERVER_GAME_START_ROUND);
}

void SitAndGo::PlayNextRound()
{
	if(HasRoundPlayers()&& m_pGame){		
		m_pGame->PlayNextRound();
		StartBlindTimer();
	}else{
		Start();
	}
}


void  
SitAndGo::StartBlindTimer()
{
	m_BlindsTimer.expires_from_now(
		boost::posix_time::seconds(m_pPokerGameInfo->blindsInfo.raise_time));
	m_BlindsTimer.async_wait(
		boost::bind(&SitAndGo::TimerExpired, this,				
			boost::asio::placeholders::error));		
}

void SitAndGo::TimerWakeUp()
{
	(this->*m_pfnNextState)();
}

void 
SitAndGo::TimerExpired(const boost::system::error_code &ec)
{
	//if error code has value then return
	if(ec) return;
	
	//send notify blind raise
	m_pPacketSender->SendAllRoomParam(eBlindRaise,0 , 
				m_ClientList);

	//increments next Stake
	m_nLevel++;
	m_fNextStake += (m_pGameInfo->stake* 
		(m_pPokerGameInfo->blindsInfo.raise_factor));

	//restart timer
	StartBlindTimer();
}

void 
SitAndGo::SetGame(boost::shared_ptr<PokerGameBase> game)
{
	m_pGame = game;
	m_pGame->SetGameHandler(this);
}

bool SitAndGo::HasRoundPlayers()
{
	int idx = 0;
	int ctr_valid=0;
	boost::shared_ptr<Table> pTable = m_pGame->GetGameTable();
	for(idx;idx<m_pGameInfo->max_players;idx++) {
		Player* player = pTable->GetPlayerAt(idx);
		if(!player) continue;
		if(player->GetTotalChips() > 0.0) {
			ctr_valid++;
		}
	}
	if(ctr_valid>= m_pGameInfo->max_players)
		return true;
	return false;
}

void SitAndGo::HandlePostGame()
{
	//set stake to next stake
	boost::shared_ptr<GameInfo> info = m_pGame->GetGameInfo();
	if( m_fNextStake != info->stake){
		info->stake = m_fNextStake;
	}
}

void SitAndGo::HandleWin() {}

void SitAndGo::BeginRound() {}

void SitAndGo::InternalInit()
{	
	m_pGame->Init();
	m_fNextStake = m_pGameInfo->stake;
	if(m_pGameInfo){
		m_pPokerGameInfo = 
			boost::static_pointer_cast<GameInfoPoker>(m_pGameInfo);
	}
}

void SitAndGo::SeatDownPlayer(boost::shared_ptr<NetPacketJoinGame> packet, 
	boost::shared_ptr<ClientSession> session)
{
	packet->SetBuyIn(m_pPokerGameInfo->startingChips);
	m_pGame->SeatDownPlayer(packet,session);
}

void  SitAndGo::HandlePacket(boost::shared_ptr<NetPacketRoom> packet,
		boost::shared_ptr<ClientSession> session)
{
	switch(packet->GetCommand()){
		case eJoin:{
			boost::shared_ptr<NetPacketJoinGame> packetJoin =
				boost::dynamic_pointer_cast<NetPacketJoinGame>(packet);
			if(packetJoin == NULL) return;
			SeatDownPlayer(packetJoin,session);
			 }break;
		default:
			m_pGame->HandlePacket(packet,session);
			break;
	}	
}

void SitAndGo::AddSession(boost::shared_ptr<ClientSession> session)
{
	GameBase::AddSession(session);
	m_pGame->AddSession(session);
}

void SitAndGo::StandUpPlayer(boost::shared_ptr<ClientSession> session)
{
	m_pGame->StandUpPlayer(session);
}

void SitAndGo::SendPlayerList( boost::shared_ptr<ClientSession> session )
{
	m_pGame->SendPlayerList(session);
}

void SitAndGo::SendGameInfo(boost::shared_ptr<ClientSession> session )
{
	m_pGame->SendGameInfo(session);
}

int SitAndGo::GetNumPlayers()
{
	return m_pGame->GetNumPlayers();
}

IDType SitAndGo::GetID() const
{
	return m_pGame->GetID();
}

void SitAndGo::LeaveRoom(boost::shared_ptr<ClientSession> session)
{
	GameBase::LeaveRoom(session);
	m_pGame->LeaveRoom(session);
}

void SitAndGo::SendTableSnapShot()
{
	m_pGame->SendTableSnapShot();
}