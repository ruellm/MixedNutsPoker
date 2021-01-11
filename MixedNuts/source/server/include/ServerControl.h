#ifndef _CPSERVER_H_
#define _CPSERVER_H_

//includes
#include <boost/asio.hpp>
#include <boost/bind.hpp>

//#include <boost/asio/ssl.hpp> 
#include <boost/enable_shared_from_this.hpp>

//Custom headers
#include "ServerLobby.h"
#include "ConfigManager.h"

//defines
#define SERVER_LOCAL_IP		"127.0.0.1"

class ServerControl 
	: public boost::enable_shared_from_this<ServerControl> 
{
public:
	ServerControl( const ServerConfig& config );

	void Initialize();

	//Run the server's io_service loop
	void Run();

	//Stop the server
	void Stop();

private:		
	
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_pAcceptor;	
	boost::shared_ptr<boost::asio::ip::tcp::endpoint> m_Endpoint;
	ServerLobby m_ServerLobby;
	ServerConfig m_Config;

	//Unable this later
	//boost::asio::ssl::context m_SSLContext;

	//private methods
	void HandleAccept(boost::shared_ptr<boost::asio::ip::tcp::socket>,
		const boost::system::error_code& e);

	void StartListen();


};

#endif