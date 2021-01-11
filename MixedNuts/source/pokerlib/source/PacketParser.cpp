#include "PacketParser.h"
#include "NetPacketFactory.h"
#include "Tokenizer.h"

using namespace std;

PacketParser::PacketParser(void){}
PacketParser::~PacketParser(void){}

void 
PacketParser::Parse( std::size_t bufflen, RecievedData& recvData )
{
	std::string strTempBuffer(recvData.recvBuffer);
	std::string strRawPacket = strTempBuffer.substr(0,bufflen);	

	string::size_type spos = 0;
	string::size_type lpos = 0;
	int startIndex = 0;
	int lastIndex = 0;
	std::string strPacket="";	

	while(startIndex<strRawPacket.size()) {
		bool bGenerate = true;

		spos = startIndex;
		lpos = strRawPacket.find_first_of(PACKET_DELIMETER, spos);
		if( lpos == string::npos) {
			lpos = strRawPacket.size();
			bGenerate = false;
		}

		strPacket = strRawPacket.substr(spos,(lpos-spos)+1);							
		startIndex =  lpos+1;	
		recvData.strIncompleteData += strPacket;

		if(bGenerate){
			lpos = recvData.strIncompleteData.find_first_of(PACKET_DELIMETER, 0);
			std::string strPacketComplete = recvData.strIncompleteData.substr(0,(lpos+1));
			recvData.strIncompleteData  = recvData.strIncompleteData.substr(lpos+1);
			Parse(strPacketComplete,recvData.recievedPacketList);
		}
	}	
}

void 
PacketParser::Parse(std::size_t bufflen, 
					char* pBuffer, NetPacketList& token_out)
{	
	std::string strTempBuffer(pBuffer);
	std::string strRawPacket = strTempBuffer.substr(0,bufflen);	
	Parse(strRawPacket,token_out);
}

void 
PacketParser::Parse(std::string strRawPacket,NetPacketList& token_out)
{
	Tokens token_list;			
	NetPacketFactory factory;
	//parse number of packets separated by "|"
	Tokenize(strRawPacket, token_list, PACKET_DELIMETER);	
	for(unsigned ctr=0;ctr<token_list.size();ctr++) {
		//parse argument
		Tokens token_argument;
		Tokenize(token_list.at(ctr), token_argument, PACKET_ARGUMENT_DELIMETER);	
		//build packet
		boost::shared_ptr<INetPacket> packet = factory.BuildFromToken(token_argument);
		if(packet == NULL) continue;
		//add to packet list
		token_out.push(packet);
	}
}