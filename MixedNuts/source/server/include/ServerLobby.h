#ifndef _SERVERLOBBY_H_
#define _SERVERLOBBY_H_

#include <map>
#include <boost/bind.hpp>
#include "ClientSession.h"
#include "definitions.h"
#include "PacketParser.h"
#include "DataManager.h"
#include "GameBase.h"
#include "GameFactory.h"
#include "PacketSender.h"
#include "NetPacketLobbyMsg.h"

class ServerLobby
{
public:
	ServerLobby();
	~ServerLobby(void);	
	
	void AddConnection(boost::shared_ptr<boost::asio::ip::tcp::socket> pSocket);

private:
	SessionIDType	m_lLastUserID;
	GameIDType		m_lLastGameID;		
	PacketParser	m_PacketParser;
	DataManager< boost::shared_ptr<ClientSession> > m_SessionManager;
	DataManager< boost::shared_ptr<GameBase> > m_GameManager;
	GameFactory			m_GameFactory;
	PacketSenderLobby	m_PacketSender;

	//private methods
	SessionIDType GetNextSessionID();	
	void HandleRead(const boost::system::error_code& error, 
		std::size_t bytes_transferred, SessionIDType sessionID );

	void HandleClientInfoReq(IDType id, boost::shared_ptr<ClientSession> session);
	void DoLogInClient( boost::shared_ptr<NetPacketReqLogIn> packet,
		boost::shared_ptr<ClientSession> session);
	void UnsubscribeToAllGames( boost::shared_ptr<ClientSession> session );
	void HandleRequest(boost::shared_ptr<ClientSession> session);
	void HandleReqPokerGameList(boost::shared_ptr<ClientSession> session, 
		boost::shared_ptr<INetPacket> packet);	
};

#endif
