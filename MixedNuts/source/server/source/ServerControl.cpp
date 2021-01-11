#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include "ServerLobby.h"
#include "ServerControl.h"
#include "IOServicePool.h"

using boost::asio::ip::tcp;

ServerControl::ServerControl(const ServerConfig& config)
	: m_Config(config)
{	

}

void ServerControl::Initialize()
{			
	IOServicePool* m_pIOServicePool = IOServicePool::GetInstance();
	m_pIOServicePool->CreatePool( m_Config.nPoolSize );
	m_pAcceptor.reset( new boost::asio::ip::tcp::acceptor(IOServicePool::GetInstance()->GetIOServices()));

	//TODO: load SSL context
	StartListen();
}

void ServerControl::StartListen()
{
	if (m_Config.bIpv6)
		m_Endpoint.reset(new tcp::endpoint(tcp::v6(), m_Config.sPort));
	else
		m_Endpoint.reset(new tcp::endpoint(tcp::v4(), m_Config.sPort));

	m_pAcceptor->open(m_Endpoint->protocol());
	m_pAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	
	if (m_Config.bIpv6) // In IPv6 mode: Be compatible with IPv4.
		m_pAcceptor->set_option(boost::asio::ip::v6_only(false));

	m_pAcceptor->bind(*m_Endpoint);
	m_pAcceptor->listen();

	boost::shared_ptr<tcp::socket> new_socket( new tcp::socket(IOServicePool::GetInstance()->GetIOServices()));
	m_pAcceptor->async_accept(*new_socket,
		  boost::bind(&ServerControl::HandleAccept, this,new_socket,
			boost::asio::placeholders::error) );
}

void ServerControl::Run()
{
	try{		
		IOServicePool::GetInstance()->Run();
	}catch(...){	
	}
}

void ServerControl::Stop()
{
	IOServicePool::GetInstance()->Stop();
}

void ServerControl::HandleAccept(boost::shared_ptr<boost::asio::ip::tcp::socket> acceptedSocket,
							const boost::system::error_code& e)
{
	if (!e)
	{
		boost::asio::socket_base::non_blocking_io command(true);
		acceptedSocket->io_control(command);
		acceptedSocket->set_option(tcp::no_delay(true));
		acceptedSocket->set_option(boost::asio::socket_base::keep_alive(true));

		m_ServerLobby.AddConnection(acceptedSocket);

		boost::shared_ptr<tcp::socket> new_socket( new tcp::socket(IOServicePool::GetInstance()->GetIOServices()));
		m_pAcceptor->async_accept(*new_socket,
			  boost::bind(&ServerControl::HandleAccept, this,new_socket,
			  boost::asio::placeholders::error));
	}
}

