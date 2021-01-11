#ifndef _RECIEVEDDATA_H_
#define _RECIEVEDDATA_H_

#include "definitions.h"
#include "INetPacket.h"
#include <list>
#include <string>

struct RecievedData
{
	NetPacketList recievedPacketList; 

	// must be dynamic to avoid stack overflow
	char* recvBuffer;	
	
	//added 03/21/2010
	std::string strIncompleteData;

	RecievedData( ) {
		recvBuffer = new char[MAX_BUFFER_SIZE];
	}
	~RecievedData( ) {
		if( recvBuffer != NULL ) {
			delete[] recvBuffer;
		}
	}
};

#endif