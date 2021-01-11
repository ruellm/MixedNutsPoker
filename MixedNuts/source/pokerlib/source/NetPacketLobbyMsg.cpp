#include <string>
#include "definitions.h"
#include "NetPacketLobbyMsg.h"
#include "StringHelper.h"

/*
 * Request room common
*/

std::string
NetPacketReqRoomList::ToString()
{
	//packet_id(space)gametype;|
	std::string strRawPacket =
          format("%d*%d;|",
		      m_sPacketID,
			  m_gameType);
    return strRawPacket;
}

/*
* Request game room POKER list base
*/

std::string 
NetPacketReqRoomPokerList::ToString()
{
	//packet_id(space)gametype;gamemode;gamelimit;|
	std::string strRawPacket =
		format("%d*%d;%d;%d;|",
			m_sPacketID,
			m_gameType,
			m_gameMode,
			m_gameLimit);
	return strRawPacket;

}


/*
 * Create Game
*/
std::string
NetPacketCreateRoom::ToString()
{
	
	if( !m_pGameInfo ) return NULL;
	//packet_id(space)game_type;stake;maxplayers;action_time_limit;|
	std::string strRawPacket =
		format("%d*%d;%f;%d;%d;%s;|",
			m_sPacketID,
			m_pGameInfo->game_type,
			m_pGameInfo->stake,
			m_pGameInfo->max_players,
			m_pGameInfo->action_time_imit,
			m_pGameInfo->strDescription.c_str());
	return strRawPacket;
}

/*
 * Create Game - POKER base
 */
std::string
NetPacketCreateRoomPoker::ToString()
{
	//if( !m_pGameInfo ) return NULL;
	////packet_id(space)game_type;stake;maxplayers;action_time_limit;mode;limit;raise_time;raise_factor|
	//boost::shared_ptr<GameInfoPoker> pGameInfo = boost::static_pointer_cast<GameInfoPoker>(m_pGameInfo);
	//
	//std::string strRawPacket =
	//	format("%d*%d;%f;%d;%d;%s;%d;%d;|",
	//		m_sPacketID,
	//		pGameInfo->game_type,
	//		pGameInfo->stake,
	//		pGameInfo->max_players,
	//		pGameInfo->action_time_imit,			
	//		m_pGameInfo->strDescription.c_str(),
	//		pGameInfo->mode,
	//		pGameInfo->limit_type);
	//return strRawPacket;
	return NULL;
}

// Game list item
std::string 
NetPacketGameListItem::ToString()
{
	//packet_id(space)parameter;|
	std::string strRawPacket =
		format("%d*%d;%d;%f;%s;%d;%d|",
			m_sPacketID,
			m_gameID,
			m_gameType,
			m_fStake,
			m_strGameDesc.c_str(),
			m_nNumPlayers,
			m_nMaxPlayers);
		
	return strRawPacket;
}
// Game list item - Poker
std::string 
NetPacketPokerGameListItem::ToString()
{
	//packet_id(space)parameter;|
	std::string strRawPacket =
		format("%d*%d;%d;%f;%s;%d;%d;%d;%f;%f;%d;%f;|",
			m_sPacketID,
			m_gameID,
			m_gameType,
			m_fStake,
			m_strGameDesc.c_str(),
			m_nNumPlayers,
			m_nMaxPlayers,
			m_gameLimit,
			m_fAntes,
			m_fBringIn,
			m_nRaiseTime,
			m_fRaiseFactor);
		
	return strRawPacket;
}
//// Game list item - seven card stud Poker
//std::string 
//NetPacketSevenCardStudGameListItem::ToString()
//{
//	//packet_id(space)parameter;|
//	std::string strRawPacket =
//		format("%d*%d;%d;%f;%s;%d;%d;%d;%f;%f;|",
//			m_sPacketID,
//			m_gameID,
//			m_gameType,
//			m_fStake,
//			m_strGameDesc.c_str(),
//			m_nNumPlayers,
//			m_nMaxPlayers,
//			m_gameLimit,
//			m_fAntes,
//			m_fBringIn);
//	return strRawPacket;
//}


/*
 * Packet for integer parameters 
*/
NetPacketInt::NetPacketInt(PacketIDType type,IDType param)
	:m_nParam(param)
{
	m_sPacketID = type;
}

std::string 
NetPacketInt::ToString()
{
	//packet_id(space)parameter;|
	std::string strRawPacket =
		format("%d*%d;|",
			m_sPacketID,
			m_nParam);
		
	return strRawPacket;
}

IDType 
NetPacketInt::GetParam() const
{
	return m_nParam;
}

/* Packet for request logedin */
NetPacketReqLogIn::NetPacketReqLogIn(): 
	m_strUserName(""),m_strPassword("")
{
	m_sPacketID = eReqLogIn;
}

std::string 
NetPacketReqLogIn::GetName() const
{
	return m_strUserName;
}

std::string 
NetPacketReqLogIn::GetPassWord() const
{
	return m_strPassword;
}

void NetPacketReqLogIn::SetUserName(std::string uname)
{ m_strUserName = uname; }

void NetPacketReqLogIn::SetPassword(std::string pass)
{ m_strPassword = pass; }

std::string 
NetPacketReqLogIn::ToString()
{	
	//packet_id(space)username;password;|
	std::string strRawPacket =
		format("%d*%s;%s;|",
			m_sPacketID,
			m_strUserName.c_str(),
			m_strPassword.c_str());
		
	return strRawPacket;
}

NetPacketClientInfoFeedBack::NetPacketClientInfoFeedBack():
	m_strUserName(""),m_strLocation(""),m_nPlayerID(0)
{
	m_sPacketID = eClientInfoFeedBack;
}

std::string 
NetPacketClientInfoFeedBack::GetName() const
{
	return m_strUserName;
}

void 
NetPacketClientInfoFeedBack::SetName(std::string uname)
{ m_strUserName = uname; }

std::string 
NetPacketClientInfoFeedBack::GetLocation() const
{
	return m_strLocation;
}

void NetPacketClientInfoFeedBack::SetLocation(std::string loc)
{ m_strLocation = loc; }

void 
NetPacketClientInfoFeedBack::SetPlayerID(IDType pID)
{ m_nPlayerID = pID; }

IDType 
NetPacketClientInfoFeedBack::GetPlayerID() const
{ return m_nPlayerID; }


std::string 
NetPacketClientInfoFeedBack::ToString()
{
	//packet_id(space)username;password;|
	std::string strRawPacket =
		format("%d*%d;%s;%s;|",
			m_sPacketID,
			m_nPlayerID,
			m_strUserName.c_str(),
			m_strLocation.c_str());
		
	return strRawPacket;
}

/* game Player list */
NetPacketPlayerList::NetPacketPlayerList():
	m_nRoomID(0)
{
	m_sPacketID = ePlayerList;
}

void NetPacketPlayerList::AddPlayer(Player* p)
{
	m_PlayerList.push_back(p);
}

void  NetPacketPlayerList::SetPlayers(std::vector<Player*>& list)
{
	 m_PlayerList = list;
}

void NetPacketPlayerList::SetRoomID(IDType id)
{
	m_nRoomID = id;
}

std::string 
NetPacketPlayerList::ToString()
{
	//packet_id(space)roomid;playerinfo;playerinfo;|
	//playerinfo = id/totalchips
	std::string strData ="%d*%d;";
	std::string strPotInfo;
	
	//collect player snapshot
	for(unsigned idx=0;idx<m_PlayerList.size();idx++){
		Player* p = m_PlayerList[idx];
		std::string strPlayer =
				format("%d/%f;",
				p->GetID(),
				p->GetTotalChips());	
		strData += strPlayer;
	}
	
	//end data
	strData += PACKET_DELIMETER;
	std::string strRawPacket =
		format(strData.c_str(),
			m_sPacketID,
			m_nRoomID);

	return strRawPacket;
}

	