#ifndef _INETPACKET_H_
#define _INETPACKET_H_

#include <string>
#include <stack>
#include <boost/shared_ptr.hpp>
#include "definitions.h"
#include "Tokenizer.h"

class INetPacket
{
public:
	virtual std::string ToString() = 0;
	inline PacketIDType GetID( ) const { return m_sPacketID; }
protected:
	PacketIDType m_sPacketID;
};

typedef std::stack< boost::shared_ptr<INetPacket> > NetPacketList;

#endif
