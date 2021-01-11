#ifndef _IOSERVICEPOOL_H_
#define _IOSERVICEPOOL_H_

#include <boost/asio.hpp>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

class IOServicePool
	: private boost::noncopyable
{
public:	
	
	/// Run all io_service objects in the pool.
	void Run();

	/// Stop all io_service objects in the pool.
	void Stop();

	/// Get an io_service to use.
	boost::asio::io_service& GetIOServices();

	//create instance and initializes pool
	void CreatePool(std::size_t pool_size);
	
	//Accessor
	static IOServicePool* GetInstance();
	static void Release();

private:
	IOServicePool(void);
	~IOServicePool(void);

	/// Construct the io_service pool.
	IOServicePool(std::size_t pool_size);

	typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
	typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

	/// The pool of io_services.
	std::vector<io_service_ptr> io_services_;

	/// The work that keeps the io_services running.
	std::vector<work_ptr> work_;

	/// The next io_service to use for a connection.
	std::size_t next_io_service_;

	static IOServicePool* m_pInstance;
	
	//pool size
	std::size_t m_nPoolSize;
};

#endif