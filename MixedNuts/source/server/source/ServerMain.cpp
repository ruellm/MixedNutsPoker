#include "ServerControl.h"
#include "ConfigManager.h"

#if defined(WIN32)
										
boost::function0<void> console_ctrl_function;
BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
  switch (ctrl_type)
  {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_CLOSE_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    console_ctrl_function();
    return TRUE;
  default:
    return FALSE;
  }
}

#elif !defined(WIN32)
	#include <pthread.h>
	#include <signal.h>
	#include <boost/thread/thread.hpp>
	#include <unistd.h>
	int daemon(int, int);
#endif 

int main(int argc, char* argv[])
{
	ConfigManager conman;	
	ServerConfig config;

	//load configuration
	if( argc < 2 )
		conman.LoadDefault(&config);
	else
		conman.LoadConfig(argv[1],&config);
	
	ServerControl* pServer = new ServerControl(config);
	pServer->Initialize();

	#if defined(WIN32)
		// Set console control handler to allow server to be stopped.
		console_ctrl_function = boost::bind(&ServerControl::Stop, pServer);
		SetConsoleCtrlHandler(console_ctrl_handler, TRUE);	
		pServer->Run();
		pServer->Stop();
	
	#elif !defined(WIN32)
		
		//set this as background process
		daemon(0, 0);

		// Block all signals for background thread. 
		//Check if bellow part is necessary
		sigset_t new_mask;
		sigfillset(&new_mask);
		sigset_t old_mask;
		pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);
		
		// Run server in background thread.
		boost::thread t(boost::bind(&ServerControl::Run, pServer));

		// Restore previous signals.
		pthread_sigmask(SIG_SETMASK, &old_mask, 0);

		// Wait for signal indicating time to shut down.
		sigset_t wait_mask;
		sigemptyset(&wait_mask);
		sigaddset(&wait_mask, SIGINT);
		sigaddset(&wait_mask, SIGQUIT);
		sigaddset(&wait_mask, SIGTERM);
		pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
		int sig = 0;
		sigwait(&wait_mask, &sig);

		 // Stop the server and wait for the background thread to exit.
		pServer->Stop();
		t.join();
	#endif
		
	delete pServer;

	return 0;
}