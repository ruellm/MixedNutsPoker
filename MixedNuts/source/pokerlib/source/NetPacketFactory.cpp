#include <stdlib.h>
#include <string>
#include "NetPacketFactory.h"
#include "NetPacketLobbyMsg.h"
#include "NetPacketGame.h"

NetPacketFactory::NetPacketFactory(void)
{
}

NetPacketFactory::~NetPacketFactory(void)
{
}

boost::shared_ptr<INetPacket> 
NetPacketFactory::BuildFromToken(const Tokens& tokens)
{
	boost::shared_ptr<INetPacket> packet;
	std::string strID = tokens.at(0);
	unsigned short usPacketID = (unsigned short)atoi(strID.c_str());
	switch(usPacketID)
	{
		case eReqGameList:
			break;
		case eReqPokerGameList:
			packet = RequestPokerGameList(tokens);
			break;
		case eCreatePokerGame:
			packet = CreatePokerGame(tokens);
			break;
		case eStandUp:
		case eLeaveRoom:
		case eGetClientInfo:
		case eReqGamePlayerList:		
		case eGameSpectate:{
			std::string strRoom = tokens.at(1);
			IDType roomID = (IDType) atoi(strRoom.c_str());
			packet.reset(new NetPacketInt((PacketIDType)usPacketID,roomID));
			}break;
		case eRoomSpecific:		//Game room specific
			packet = CreateRoomSpecific(tokens);
			break;	
		case eReqLogIn:
			packet = CreateLogIn(tokens);
			break;		

	}
	return packet;
}

boost::shared_ptr<INetPacket> 
NetPacketFactory::CreateLogIn(const Tokens& tokens)
{
	Tokens param;
	boost::shared_ptr<NetPacketReqLogIn> login(new NetPacketReqLogIn());
	std::string strParamTok = tokens.at(1);
	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);		
	
	login->SetUserName(param.at(0));
	if(param.size()>1)
		login->SetPassword(param.at(1));
	return login;
}


boost::shared_ptr<INetPacket> 
NetPacketFactory::CreatePokerGame(const Tokens& tokens)
{
	Tokens param;
	boost::shared_ptr<NetPacketCreateRoomPoker> game(new NetPacketCreateRoomPoker());
	std::string strParamTok = tokens.at(1);
	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);
	boost::shared_ptr<GameInfoPoker> pGameInfo(new GameInfoPoker());

	//GameType type = (GameType) atoi(param.at(0).c_str());
	//GameModeType mode = (GameModeType) atoi(param.at(5).c_str());
	
	/*if( type == eTexasHoldem || 
		type == eOmahaHoldem )
	{		
		if(mode == eModeSNG){
			pGameInfo.reset(new GameInfoSng());			
			boost::shared_ptr<GameInfoSng> pGameInfoSng =
				boost::static_pointer_cast<GameInfoSng>(pGameInfo);
			pGameInfoSng->blindsInfo.raise_time = atof (param.at(7).c_str());
			pGameInfoSng->blindsInfo.raise_factor = atof (param.at(8).c_str());
			pGameInfoSng->startingChips = atof (param.at(8).c_str());

		}else{
			pGameInfo.reset(new GameInfoPoker());
		}
	}	
	else if(type == eSevenCardStud)
	{

		pGameInfo.reset(new GameInfoSevenStud());
		boost::shared_ptr<GameInfoSevenStud> pGameInfoStud =
			boost::static_pointer_cast<GameInfoSevenStud>(pGameInfo);
		pGameInfoStud->fAntes = atof (param.at(7).c_str());
		pGameInfoStud->fBringIn = atof (param.at(8).c_str());
	}	*/

	pGameInfo->game_type		= (GameType) atoi(param.at(0).c_str());
	pGameInfo->stake			= (float)atof(param.at(1).c_str());														
	pGameInfo->max_players		= atoi(param.at(2).c_str());			
	pGameInfo->action_time_imit = atoi(param.at(3).c_str());		
	pGameInfo->strDescription	= param.at(4);								
	pGameInfo->mode				= (GameModeType) atoi(param.at(5).c_str());
	pGameInfo->limit_type		= (GameLimitType) atoi(param.at(6).c_str());
	
	//commonly used in 7 card stud
	pGameInfo->fAntes = (float)atof (param.at(7).c_str());
	pGameInfo->fBringIn = (float)atof (param.at(8).c_str());
	
	//only used in SNG games but must not be empty
	pGameInfo->blindsInfo.raise_time = atoi (param.at(9).c_str());
	pGameInfo->blindsInfo.raise_factor = (float)atof (param.at(10).c_str());
	pGameInfo->startingChips = (float)atof (param.at(11).c_str());				

		
	game->SetGameInfo(pGameInfo);
	return game;
}
	

boost::shared_ptr<INetPacket> 
NetPacketFactory::CreatePacketJoin(const Tokens& tokens)
{
	Tokens param;
	std::string strParamTok = tokens.at(2);
	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);	

	std::string strRoom		= param.at(0);
	std::string strSeat		= param.at(1);
	std::string strBuyIn	= param.at(2);
	IDType nRoom			= (IDType) atoi(strRoom.c_str());
	short nSeat				= (IDType) atoi(strSeat.c_str());	
	float fBuyin			= (float)atof(strBuyIn.c_str());

	boost::shared_ptr<NetPacketJoinGame> game( new NetPacketJoinGame());
	game->SetRoom(nRoom);
	game->SetSeatNum(nSeat);
	game->SetBuyIn(fBuyin);

	return game;
}

boost::shared_ptr<INetPacket> 
NetPacketFactory::CreateRoomSpecific(const Tokens& tokens)
{
	boost::shared_ptr<INetPacket> game;
	std::string strCmd = tokens.at(1);
	unsigned short usCmd = (unsigned short)atoi(strCmd.c_str());
	
	switch(usCmd) {
		case eJoin:{
			game = CreatePacketJoin(tokens);
		}break;
		case eAction:
			game = CreateAction(tokens);
			break;
		case eChatMsg:
			game = CreateChatMsg(tokens);
			break;
		case eShowHands:
			game = CreateRoomInt(tokens);
			break;
	}

	return game;
}
boost::shared_ptr<INetPacket> 
NetPacketFactory::CreateRoomInt(const Tokens& tokens)
{
	Tokens param;
	std::string strParamTok = tokens.at(2);	
	GamePacket packetID = (GamePacket)atoi(tokens.at(1).c_str());

	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);	

	IDType nRoom = atoi(param.at(0).c_str());
	int nParam = atoi(param.at(1).c_str());

	boost::shared_ptr<NetPacketRoomInt> packet( new NetPacketRoomInt(packetID,nParam));
	packet->SetRoom(nRoom);

	return packet;
}
boost::shared_ptr<INetPacket> 
NetPacketFactory::CreateChatMsg(const Tokens& tokens)
{
	Tokens param;
	std::string strParamTok = tokens.at(2);
	boost::shared_ptr<NetPacketChatMsg> chat( new NetPacketChatMsg());
	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);	

	std::string strRoom	= param.at(0);
	std::string strPID = param.at(1);
	IDType nRoom = (IDType) atoi(strRoom.c_str());
	IDType nPID = (IDType) atoi(strPID.c_str());

	chat->SetRoom(nRoom);
	chat->SetPlayerID(nPID);
	chat->SetMsg(param.at(2));

	return chat;
}

boost::shared_ptr<INetPacket> 
NetPacketFactory::CreateAction(const Tokens& tokens)
{
	Tokens param;
	std::string strParamTok = tokens.at(2);
	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);	

	std::string strRoom		= param.at(0);
	std::string strAction	= param.at(1);
	std::string strAmount	= param.at(2);
	//std::string strPlayerID = param.at(2);

	IDType nRoom		 = (IDType) atoi(strRoom.c_str());
	PokerAction action	 = (PokerAction) atoi(strAction.c_str());	
	float fAmount		 = (float)atof(strAmount.c_str());
	//IDType id			 = (IDType) atoi(strPlayerID.c_str());

	boost::shared_ptr<NetPacketAction> game( new NetPacketAction(action,fAmount,nRoom));
	return game;

}

boost::shared_ptr<INetPacket>
NetPacketFactory::RequestPokerGameList(const Tokens& tokens)
{
	Tokens param;
	boost::shared_ptr<NetPacketReqRoomPokerList> game(new NetPacketReqRoomPokerList());
	std::string strParamTok = tokens.at(1);
	Tokenize(strParamTok, param, PACKET_PARAMETER_DELIMETER);		
	
	game->SetGameType((GameType)atoi(param.at(0).c_str()));
	game->SetGameMode((GameModeType)atoi(param.at(1).c_str()));
	game->SetGameLimit((GameLimitType)atoi(param.at(2).c_str()));

	return game;
}
