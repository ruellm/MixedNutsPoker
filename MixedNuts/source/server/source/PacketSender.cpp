#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>
#include "PacketSender.h"
#include "NetPacketLobbyMsg.h"
#include "PokerGameBase.h"

PacketSender::PacketSender(void)
{
}

PacketSender::~PacketSender(void)
{
}

void PacketSender::Send(boost::shared_ptr<INetPacket> packet,
		boost::shared_ptr<ClientSession> session)
{
	if( packet == NULL || session == NULL ) return;
	
	//Get buffer string
	m_strRawData = packet->ToString();
	boost::asio::async_write( *session->GetSocket(),
		boost::asio::buffer(m_strRawData),
		boost::bind(&PacketSender::HandleWrite, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void PacketSender::HandleWrite(const boost::system::error_code& error,
      size_t /*bytes_transferred*/)
{ }

void 
PacketSenderLobby::SendPacketInt(PacketIDType packetType,IDType param,
								 boost::shared_ptr<ClientSession> session)
{
	boost::shared_ptr<NetPacketInt> packet( new NetPacketInt(packetType,param) );
	Send( packet, session);
}

void 
PacketSenderLobby::SendClientInfo(boost::shared_ptr<ClientSession> client,
		boost::shared_ptr<ClientSession> session)
{
	boost::shared_ptr<NetPacketClientInfoFeedBack> packet( new NetPacketClientInfoFeedBack() );
	packet->SetPlayerID(client->GetSessionID());
	packet->SetName(client->GetName());
	packet->SetLocation(client->GetLocation());
	Send( packet, session);
}
