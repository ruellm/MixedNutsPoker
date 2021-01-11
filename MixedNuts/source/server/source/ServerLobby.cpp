#include <boost/pointer_cast.hpp>
#include "ServerLobby.h"
#include "NetPacketLobbyMsg.h"
#include "PacketSender.h"
#include "PokerGameBase.h"

ServerLobby::ServerLobby()
	:m_lLastUserID(CLIENT_SESSION_ID_BASE)
{
}

ServerLobby::~ServerLobby(void)
{
}

void 
ServerLobby::AddConnection(boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket)
{
	boost::shared_ptr<ClientSession> new_client(new ClientSession(pSocket,GetNextSessionID()));
	m_SessionManager.Add(new_client->GetSessionID(),new_client);
	
	pSocket->async_read_some(
		boost::asio::buffer(new_client->GetRecievedData().recvBuffer,MAX_BUFFER_SIZE),
		boost::bind(&ServerLobby::HandleRead,
			this,
			boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,
			new_client->GetSessionID()));
}


SessionIDType
ServerLobby::GetNextSessionID()
{
	return m_lLastUserID++;
}

void 
ServerLobby::HandleRead( const boost::system::error_code& error, 
						std::size_t bytes_transferred, SessionIDType sessionID )
{
	boost::shared_ptr<ClientSession> client = m_SessionManager.GetDataByID(sessionID);
	if(!error) {		
		if(!client) return;

		//parse the buffer read and build packets
		m_PacketParser.Parse(bytes_transferred, client->GetRecievedData());	
		HandleRequest(client);

		client->GetSocket()->async_read_some(
			boost::asio::buffer(client->GetRecievedData().recvBuffer,MAX_BUFFER_SIZE),
			boost::bind(&ServerLobby::HandleRead,
				this,
				boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,
				client->GetSessionID()
				));

	} else {
		//Remove client from list
		//TODO Remove player from game list if necessary
		m_SessionManager.Remove(sessionID);
		UnsubscribeToAllGames(client);		
	}
}

void 
ServerLobby::UnsubscribeToAllGames( boost::shared_ptr<ClientSession> session )
{
	//Remove session in all games
	m_GameManager.BeginIterate();	
	while( !m_GameManager.IsIteratorEnd() ){		
		boost::shared_ptr<GameBase> game = m_GameManager.GetNextData();
		game->LeaveRoom(session);
	}
	m_GameManager.EndIterate();	
}

void 
ServerLobby::HandleRequest(boost::shared_ptr<ClientSession> session)
{
	NEXT_PACKET:
	boost::shared_ptr<INetPacket> packet = session->GetRecievedData().recievedPacketList.top();
	session->GetRecievedData().recievedPacketList.pop();

	if(packet == NULL) return;

	if(packet->GetID() == eRoomSpecific) {
		//Handle room specific request
		boost::shared_ptr<NetPacketRoom> packetRoom = boost::dynamic_pointer_cast<NetPacketRoom>(packet);
		boost::shared_ptr<GameBase> game = m_GameManager.GetDataByID(packetRoom->GetRoomID());
		if(!game) goto END;
		game->HandlePacket(packetRoom,session);
	}else {
		//handle lobby-only request
		switch( packet->GetID() )
		{
			case eReqPokerGameList:
				HandleReqPokerGameList(session,packet);
			break;
			case eCreatePokerGame:
			case eCreateGame:{
				boost::shared_ptr<NetPacketCreateRoom> packetCreate =
					boost::dynamic_pointer_cast<NetPacketCreateRoom>(packet);
				boost::shared_ptr<GameBase> game = m_GameFactory.CreateGame(packetCreate->GetGameInfo() );
				if( game == NULL ) goto END;				

				//TODO: TBD if automatic start of game is necessary,tournament::start should be handled by TournamentManager
				game->Init();
				game->Start();
				m_GameManager.Add(game->GetID(),game);				

			}break;
			case eGameSpectate:{
				boost::shared_ptr<NetPacketInt> packetReq = 
					boost::dynamic_pointer_cast<NetPacketInt>(packet);
				boost::shared_ptr<GameBase> game = m_GameManager.GetDataByID(packetReq->GetParam());
				if(!game) goto END;
				game->AddSession(session);
			}break;			
			case eStandUp:{
				boost::shared_ptr<NetPacketInt> packetReq = 
					boost::dynamic_pointer_cast<NetPacketInt>(packet);
				boost::shared_ptr<GameBase> game = m_GameManager.GetDataByID(packetReq->GetParam());
				if(!game) goto END;
				game->StandUpPlayer(session);
			}break;		
			case eLeaveRoom:{
				boost::shared_ptr<NetPacketInt> packetReq = 
					boost::dynamic_pointer_cast<NetPacketInt>(packet);
				boost::shared_ptr<GameBase> game = m_GameManager.GetDataByID(packetReq->GetParam());
				if(!game) goto END;
				game->LeaveRoom(session);
			}break;
			case eReqLogIn:{
				boost::shared_ptr<NetPacketReqLogIn> packetLog = 
						boost::dynamic_pointer_cast<NetPacketReqLogIn>(packet);
				//TODO: dummy function, read from db later
				DoLogInClient(packetLog,session);
				}break;
			case eGetClientInfo:{
				boost::shared_ptr<NetPacketInt> packetReq = 
					boost::dynamic_pointer_cast<NetPacketInt>(packet);			
				HandleClientInfoReq(packetReq->GetParam(),session);
				}break;
			case eReqGamePlayerList:
				boost::shared_ptr<NetPacketInt> packetReq = 
					boost::dynamic_pointer_cast<NetPacketInt>(packet);
				boost::shared_ptr<GameBase> game = m_GameManager.GetDataByID(packetReq->GetParam());
				if(!game) goto END;
				game->SendPlayerList(session);
				break;
		}
	}

END:
	//cleanup packet after usage
	packet.reset();

	//TODO:TBD might cause CPU traffic
	//added 02/23/2010
	if(session->GetRecievedData().recievedPacketList.size()>0){		
		//Add delay to make way for Other CPU usage
		//TODO: TBD
		//SERVER_SLEEP(SERVER_NEXT_PACKE_DELAY); 
		goto NEXT_PACKET;
	}
}
void 
ServerLobby::HandleClientInfoReq(IDType id, boost::shared_ptr<ClientSession> session)
{	
	boost::shared_ptr<ClientSession> client = m_SessionManager.GetDataByID(id);
	if(client == NULL) return;
	m_PacketSender.SendClientInfo(client,session);
}

void ServerLobby::DoLogInClient( boost::shared_ptr<NetPacketReqLogIn> packet,
								boost::shared_ptr<ClientSession> session)
{
	//TODO: temporary validation of username
	//read from db later
	StatusType suc = eSuccess;
	m_SessionManager.BeginIterate();	
	while( !m_SessionManager.IsIteratorEnd() ){		
		boost::shared_ptr<ClientSession> client = m_SessionManager.GetNextData();	
		if(client->GetName() == packet->GetName() ){
			suc = eError;
			break;
		}
	}
	m_SessionManager.EndIterate();		
	m_PacketSender.SendPacketInt(eRegLogInFeedback,(IDType)suc,session);
	if(suc){
		session->SetName(packet->GetName());
	}
}

void ServerLobby::HandleReqPokerGameList(boost::shared_ptr<ClientSession> session, 
		boost::shared_ptr<INetPacket> packet)
{

	boost::shared_ptr<NetPacketReqRoomPokerList> pack = 
				boost::dynamic_pointer_cast<NetPacketReqRoomPokerList>(packet);
	m_GameManager.BeginIterate();	
	while( !m_GameManager.IsIteratorEnd() ){		
		boost::shared_ptr<PokerGameBase> game = boost::dynamic_pointer_cast<PokerGameBase>(m_GameManager.GetNextData());	
		boost::shared_ptr<GameInfoPoker> info = 
			boost::static_pointer_cast<GameInfoPoker>(game->GetGameInfo());

		if( pack->GetGameType() == info->game_type &&
			pack->GetGameMode() == info->mode &&
			pack->GetGameLimit() == info->limit_type){
			game->SendGameInfo(session);
		}
	}
	m_GameManager.EndIterate();	

}