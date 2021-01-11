#ifndef _PACKETSENDER_H_
#define _PACKETSENDER_H_

#include <boost/shared_ptr.hpp>
#include <string>
#include "definitions.h"
#include "ClientSession.h"
#include "INetPacket.h"

//forward declaration
class GameBase;
class PokerGameBase;

class PacketSender
{
public:
	PacketSender(void);
	~PacketSender(void);
	
	void Send(boost::shared_ptr<INetPacket> packet,
		boost::shared_ptr<ClientSession> session);
protected:
	std::string m_strRawData;

	void HandleWrite(const boost::system::error_code&,/*error*/
      size_t );/*bytes_transferred*/
};

class PacketSenderLobby: public PacketSender
{
public:	
	void SendPacketInt(PacketIDType packetType,IDType param,
		boost::shared_ptr<ClientSession> session);
	void SendClientInfo(boost::shared_ptr<ClientSession> client,
		boost::shared_ptr<ClientSession> session);
};

#endif