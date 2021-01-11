#ifndef _PACKETPARSER_H_
#define _PACKETPARSER_H_

#include <vector>
#include <boost/shared_ptr.hpp>
#include "INetPacket.h"
#include "RecievedData.h"
#include <string>

class PacketParser
{
public:
	PacketParser(void);
	~PacketParser(void);
	void Parse( std::size_t bufflen, RecievedData& recvData );
	void Parse(std::size_t bufflen, char* pBuffer, NetPacketList& token_out);
	void Parse(std::string strData,NetPacketList& token_out);
private:

	
};

#endif