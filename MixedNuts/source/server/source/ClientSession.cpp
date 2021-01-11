#include "ClientSession.h"

#include <boost/asio.hpp>

ClientSession::ClientSession(boost::shared_ptr<boost::asio::ip::tcp::socket> socket,SessionIDType lSessionId)
: m_pSocket(socket),m_lSessionID(lSessionId),m_strUserName(""),m_strLocation("")
{
}

ClientSession::~ClientSession(void)
{
}

boost::shared_ptr<boost::asio::ip::tcp::socket>
ClientSession::GetSocket() const {
	return m_pSocket;
}

SessionIDType 
ClientSession::GetSessionID( ) const
{
	return m_lSessionID;
}

RecievedData& 
ClientSession::GetRecievedData()
{
	return m_recvData;
}

std::string 
ClientSession::GetName() const
{ return m_strUserName; }

void 
ClientSession::SetName(std::string uname)
{ m_strUserName = uname; }

std::string 
ClientSession::GetLocation() const
{
	return m_strLocation;
}
