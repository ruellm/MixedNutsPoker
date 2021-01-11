#include <boost/bind.hpp>
#include "GameBase.h"
#include "IOServicePool.h"
#include "Player.h"

GameBase::GameBase(void):
	m_gameTimer(IOServicePool::GetInstance()->GetIOServices())
{
}

GameBase::~GameBase(void)
{
}

GameBase::GameBase(GameIDType id):m_gameID(id),
m_gameTimer(IOServicePool::GetInstance()->GetIOServices())
{
	
}

GameBase::GameBase(GameIDType id,const GameInfo& gameInfo):
	m_gameID(id),
	m_gameTimer(IOServicePool::GetInstance()->GetIOServices())
{
	
}
void GameBase::Init()
{
	m_pPacketSender.reset( new PacketSenderGame(m_gameID) );
	InternalInit();
}

boost::asio::deadline_timer& 
GameBase::GetStateTimer()  
{
	return m_gameTimer;
}


//TODO: this is not final
bool 
GameBase::HasRoundPlayers( )
{
	int idx = 0;
	const int players_to_play=2;
	int ctr_valid=0;
	for(idx;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player) continue;
		if(player->GetTotalChips() > 0.0) {
			ctr_valid++;
		}
	}
	if(ctr_valid>= players_to_play)
		return true;
	return false;
}

bool GameBase::HasValidPlayers( )
{
	int idx = 0;
	const int players_to_play=2;
	int ctr_valid=0;
	for(idx;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(player!=NULL){
			ctr_valid++;
		}
	}
	if(ctr_valid>= players_to_play)
		return true;
	return false;
}

IDType 
GameBase::GetID() const
{
	return m_gameID;
}


void 
GameBase::AddSession(boost::shared_ptr<ClientSession> session)
{
	m_ClientList.Add(session->GetSessionID(),session);
	//TODO: TBD update table snapshot
	SendTableSnapShot();
}

void 
GameBase::HandlePacket(boost::shared_ptr<NetPacketRoom> packet,
					   boost::shared_ptr<ClientSession> session)
{
	switch(packet->GetCommand()){
		case eJoin:{
			boost::shared_ptr<NetPacketJoinGame> packetJoin = boost::dynamic_pointer_cast<NetPacketJoinGame>(packet);
			if(packetJoin == NULL) return;
			SeatDownPlayer(packetJoin,session);
			 }break;
		case eChatMsg:{
			boost::shared_ptr<NetPacketChatMsg> packet_chat = boost::dynamic_pointer_cast<NetPacketChatMsg>(packet);
			IDType pID = packet_chat->GetPlayerID();
			//original sender sends blank id, client populate FROM ID
			packet_chat->SetPlayerID((packet_chat->GetPlayerID()==CLIENT_ID_INVALID)?session->GetSessionID():pID); 
			m_pPacketSender->SendPacketToAll(packet,m_ClientList);
			}break;
		case eReqGamePlayerList:{
			SendPlayerList(session);
			}break;
		default:
			InternalHandlePacket(packet,session);
			break;
	}

}

void 
GameBase::SeatDownPlayer(boost::shared_ptr<NetPacketJoinGame> packet, boost::shared_ptr<ClientSession> session)
{
	if(!m_pGameTable->IsSeatVacant(packet->GetSeatNum())) return;

	//Create Player and assign blinds and chips
	Player* player = new Player(session->GetSessionID());
	player->SetTotalChips(packet->GetBuyIn());
	player->SetIsPlaying(false);
	m_pGameTable->SeatDownPlayer(packet->GetSeatNum(),player);
	
	//Update all client snapshot
	SendTableSnapShot();
}
void 
GameBase::WaitForTimer(unsigned msecs)
{
	// Restart timer
	m_gameTimer.expires_from_now(
		boost::posix_time::milliseconds(msecs));
	m_gameTimer.async_wait(
		boost::bind(
			&GameBase::TimerTrigger, this,				
			boost::asio::placeholders::error));
}

void 
GameBase::WaitForTimerSync(unsigned msecs)
{
	m_gameTimer.expires_from_now(
		boost::posix_time::milliseconds(msecs));
	m_gameTimer.wait();
}

void
GameBase::TimerTrigger(const boost::system::error_code &ec)
{
	if(ec) return;	
	TimerWakeUp();
}

	//Game Info
void 
GameBase::SetGameInfo(boost::shared_ptr<GameInfo> gInfo){
	m_pGameInfo = gInfo;
}

boost::shared_ptr<GameInfo> 
GameBase::GetGameInfo() const
{
	return m_pGameInfo;
} 

boost::shared_ptr<Table> 
GameBase::GetGameTable() const
{ return m_pGameTable; }

void 
GameBase::StandUpPlayer(boost::shared_ptr<ClientSession> session)
{
	//find player seat num
	if( m_pGameTable )
		m_pGameTable->StandUpPlayer(session->GetSessionID());

	//TODO:TBD Update all client snapshot
	SendTableSnapShot();
}

void 
GameBase::LeaveRoom(boost::shared_ptr<ClientSession> session)
{
	if( m_pGameTable )
		m_pGameTable->StandUpPlayer(session->GetSessionID());
	m_ClientList.Remove(session->GetSessionID());
}

void 
GameBase::SendPlayerList( boost::shared_ptr<ClientSession> session )
{
	std::vector<Player*> playerList;
	for(int idx=0;idx<m_pGameInfo->max_players;idx++) {
		Player* player = m_pGameTable->GetPlayerAt(idx);
		if(!player) continue;
		playerList.push_back(player);
	}
	m_pPacketSender->SendPlayerList(playerList,session);
}


void GameBase::SendGameInfo(boost::shared_ptr<ClientSession> session )
{
	//blank
}