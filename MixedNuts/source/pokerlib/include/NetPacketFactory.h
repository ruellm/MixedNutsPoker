#ifndef _NETPACKETFACTORY_H_
#define _NETPACKETFACTORY_H_

#include <boost/shared_ptr.hpp>
#include "definitions.h"
#include "INetPacket.h"
#include "PacketParser.h"
#include "Tokenizer.h"

class NetPacketFactory
{
public:
	NetPacketFactory(void);
	~NetPacketFactory(void);
	boost::shared_ptr<INetPacket> BuildFromToken(const Tokens& tokens);
private:
	boost::shared_ptr<INetPacket> CreateGame(const Tokens& tokens);

	boost::shared_ptr<INetPacket> CreatePacketJoin(const Tokens& tokens);
	boost::shared_ptr<INetPacket> CreateRoomSpecific(const Tokens& tokens);
	boost::shared_ptr<INetPacket> CreateAction(const Tokens& tokens);
	boost::shared_ptr<INetPacket> CreatePokerGame(const Tokens& tokens);
	boost::shared_ptr<INetPacket> RequestPokerGameList(const Tokens& tokens);	
	boost::shared_ptr<INetPacket> CreateLogIn(const Tokens& tokens);	
	boost::shared_ptr<INetPacket> CreateChatMsg(const Tokens& tokens);		
	boost::shared_ptr<INetPacket> CreateRoomInt(const Tokens& tokens);		

};

#endif