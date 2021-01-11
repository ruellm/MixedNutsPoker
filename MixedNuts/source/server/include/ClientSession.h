#ifndef _CLIENTSESSION_H_
#define _CLIENTSESSION_H_

#include <boost/asio.hpp>
#include "definitions.h"
#include "RecievedData.h"


class ClientSession
{
public:
	ClientSession(boost::shared_ptr<boost::asio::ip::tcp::socket>,
		SessionIDType lSessionId);
	~ClientSession(void);
	
	boost::shared_ptr<boost::asio::ip::tcp::socket>	GetSocket() const;
	
	SessionIDType GetSessionID( ) const;
	RecievedData& GetRecievedData();
	std::string GetName() const;
	std::string GetLocation() const;
	void SetName(std::string uname);
	
private:
	std::string m_strUserName;					//TODO: TBD;Used to determine if logged-in or not
	std::string m_strLocation;
	SessionIDType m_lSessionID;	
	RecievedData m_recvData;

	// Socket for the connection.
	boost::shared_ptr<boost::asio::ip::tcp::socket> m_pSocket;		
};

typedef std::map<SessionIDType, boost::shared_ptr<ClientSession> > ClientSessionList;
typedef boost::shared_ptr<ClientSession> ClientSessionPtr;

#endif
