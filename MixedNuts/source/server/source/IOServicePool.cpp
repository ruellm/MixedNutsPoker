#include "IOServicePool.h"

#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

//initialize to NULL
IOServicePool* IOServicePool::m_pInstance = NULL;

IOServicePool::IOServicePool(void):
	next_io_service_(0),m_nPoolSize(1)
{

}

IOServicePool::~IOServicePool(void)
{
}

IOServicePool::IOServicePool(std::size_t pool_size)
	: next_io_service_(0),m_nPoolSize(pool_size)
{
	
}

void IOServicePool::CreatePool(std::size_t pool_size)
{
	if (pool_size == 0)
		throw std::runtime_error("IOServicePool size is 0");

	// Give all the io_services work to do so that their run() functions will not
	// exit until they are explicitly stopped.
	m_nPoolSize = pool_size;
	for (std::size_t i = 0; i < pool_size; ++i)
	{
		io_service_ptr io_service(new boost::asio::io_service);
		work_ptr work(new boost::asio::io_service::work(*io_service));
		io_services_.push_back(io_service);
		work_.push_back(work);
	}
}

IOServicePool*
IOServicePool::GetInstance()
{
	if( m_pInstance == NULL )
	{
		m_pInstance = new IOServicePool();
	}
	return m_pInstance;
}

void IOServicePool::Release()
{
	if(m_pInstance!=NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void IOServicePool::Run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < io_services_.size(); ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, io_services_[i])));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void IOServicePool::Stop()
{
  // Explicitly stop all io_services.
  for (std::size_t i = 0; i < io_services_.size(); ++i)
    io_services_[i]->stop();
}

boost::asio::io_service& IOServicePool::GetIOServices()
{
  // Use a round-robin scheme to choose the next io_service to use.
  boost::asio::io_service& io_service = *io_services_[next_io_service_];
  ++next_io_service_;
  if (next_io_service_ == io_services_.size())
    next_io_service_ = 0;
  return io_service;
}