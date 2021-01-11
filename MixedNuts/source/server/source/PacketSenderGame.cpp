#include "PacketSenderGame.h"
#include "NetPacketGame.h"
#include "NetPacketLobbyMsg.h"
#include "PokerGameBase.h"

PacketSenderGame::PacketSenderGame(void)
{}

PacketSenderGame::PacketSenderGame(IDType id)
	:m_RoomID(id)
{
}

PacketSenderGame::~PacketSenderGame(void)
{
}

void 
PacketSenderGame::SendHoleCard( const std::vector<Card>& hole,
		boost::shared_ptr<ClientSession> session)
{
	Send( boost::shared_ptr<NetPacketCards>(new NetPacketCards(eHoleCard,m_RoomID,hole)), session);	
}

void 
PacketSenderGame::SendCards( GamePacket id, const std::vector<Card>& cards,
		boost::shared_ptr<ClientSession> session)
{
	Send( boost::shared_ptr<NetPacketCards>(new NetPacketCards(id,m_RoomID,cards)), session);	
}

void PacketSenderGame::SendFlop( const std::vector<Card>& cards,
		DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketCards> packet(new NetPacketCards(eFlop,m_RoomID,cards));	
	SendPacketToAll(packet,clientList);
}

void PacketSenderGame::SendCard( GamePacket id, const Card& card, 
		DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	std::vector<Card> cards;	
	cards.push_back(card);

	boost::shared_ptr<NetPacketCards> packet(new NetPacketCards(id,m_RoomID,cards));
	SendPacketToAll(packet,clientList);
}

void 
PacketSenderGame::SetRoomID(IDType id)
{
	m_RoomID = id;
}

//void 
//PacketSenderGame::SendTableHoldemSnapShot(boost::shared_ptr<TableHoldem> table,
//										  boost::shared_ptr<ClientSession> session)
//{
//	Send( boost::shared_ptr<NetPacketHoldemTableSnapShot>(new NetPacketHoldemTableSnapShot(table,m_RoomID)), session);	
//}

void 
PacketSenderGame::SendAllTablePokerSnapShot(boost::shared_ptr<Table> table,
		DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketPokerTableSnapShot> packet(new NetPacketPokerTableSnapShot(table,m_RoomID));
	SendPacketToAll(packet,clientList);
}

//void PacketSenderGame::SendAllTableHoldemSnapShot(boost::shared_ptr<TableHoldem> table,
//		DataManager<boost::shared_ptr<ClientSession>>& clientList)
//{
//	boost::shared_ptr<NetPacketHoldemTableSnapShot> packet(new NetPacketHoldemTableSnapShot(table,m_RoomID));
//	SendPacketToAll(packet,clientList);
//}

void 
PacketSenderGame::SendWaitAction(boost::shared_ptr<ClientSession> session)
{
	Send(boost::shared_ptr<NetPacketWaitBet>(new NetPacketWaitBet(m_RoomID)),session);
}

void 
PacketSenderGame::SendPacketToAll(boost::shared_ptr<INetPacket> packet, 
		DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	clientList.BeginIterate();
	
	while( !clientList.IsIteratorEnd() ){
		boost::shared_ptr<ClientSession> session = clientList.GetNextData();					
		Send(packet, session);			
	}

	clientList.EndIterate();
}

void PacketSenderGame::SendWinnerToAll(std::vector<HandStrength>& winers,
						short potnum,float amnt,
						DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketWinner> packet( new NetPacketWinner());
	std::vector<IDType> plist;
	for(int idx=0;idx<winers.size();idx++){
		plist.push_back(winers[idx].GetPlayerID());
	}

	packet->SetPlayerList(plist);
	packet->SetPotNum(potnum);
	packet->SetRoom(m_RoomID);
	packet->SetAmount(amnt);
	if( winers.size() > 0 ){
		packet->SetHandRank(winers[0].GetRanking());
		packet->SetHands(winers[0].Cards);
	}else{
		packet->SetHandRank(HandStrength::NoRank);
	}

	SendPacketToAll(packet,clientList);
}


void 
PacketSenderGame::SendHandToAll(std::vector<Card>& hands,IDType id,
		DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketShowHands> packet( new NetPacketShowHands());	
	packet->SetPlayerID(id);
	packet->SetHands(hands);
	packet->SetRoom(m_RoomID);

	SendPacketToAll(packet,clientList);
}

	
void PacketSenderGame::SendMsgToAll(const std::string& msg, IDType pID,
	DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketChatMsg> packet( new NetPacketChatMsg());	
	packet->SetRoom(m_RoomID);
	packet->SetMsg(msg);
	packet->SetPlayerID(pID);

	SendPacketToAll(packet,clientList);
}

void 
PacketSenderGame::SendPlayerList(std::vector<Player*>& playerList,
		boost::shared_ptr<ClientSession> session)
{
	boost::shared_ptr<NetPacketPlayerList> packet( new NetPacketPlayerList());		
	packet->SetPlayers(playerList);
	packet->SetRoomID(m_RoomID);
	Send(packet, session);	
}

void PacketSenderGame::SendOpenCard(std::vector<CardOwn>& ownerList,GamePacket id,
		DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketOpenCards> packet( new NetPacketOpenCards(id));	
	packet->SetRoom(m_RoomID);
	packet->SetCardList(ownerList);

	SendPacketToAll(packet,clientList);
}

void 
PacketSenderGame::SendAllRoomParam(GamePacket id, int param, 
				DataManager< boost::shared_ptr<ClientSession> >& clientList)
{
	boost::shared_ptr<NetPacketRoomInt> packet( new NetPacketRoomInt(id,param));
	packet->SetRoom(m_RoomID);
	SendPacketToAll(packet,clientList);
}

void PacketSenderGame::SendGameInfo(boost::shared_ptr<GameBase> game, 
									 boost::shared_ptr<ClientSession> session)
{

	boost::shared_ptr<NetPacketGameListItem> packet(new NetPacketGameListItem());
	packet->SetGameID(game->GetID());
	packet->SetStake(game->GetGameInfo()->stake);
	packet->SetGameDesc(game->GetGameInfo()->strDescription);
	Send( packet, session);	
}

void PacketSenderGame::SendPokerGameInfo(boost::shared_ptr<PokerGameBase> game,
		boost::shared_ptr<ClientSession> session)
{
	boost::shared_ptr<GameInfoPoker> info = 
			boost::static_pointer_cast<GameInfoPoker>(game->GetGameInfo());

	boost::shared_ptr<NetPacketPokerGameListItem> packet(new NetPacketPokerGameListItem());
	packet->SetGameID(game->GetID());
	packet->SetGameType(info->game_type);
	packet->SetStake(info->stake);
	packet->SetGameDesc(info->strDescription);
	packet->SetGameLimit(info->limit_type);
	packet->SetNumPlayers(game->GetNumPlayers());
	packet->SetMaxPlayers(info->max_players);
	packet->SetAntes(info->fAntes);
	packet->SetBringIn(info->fBringIn);
	packet->SetRaiseFactor(info->blindsInfo.raise_factor);
	packet->SetRaiseTime(info->blindsInfo.raise_time);
	
	Send( packet, session);
}
//
//void PacketSenderGame::SendSevenCardStudGameInfo(boost::shared_ptr<PokerGameBase> game,
//		boost::shared_ptr<ClientSession> session)
//{
//	boost::shared_ptr<GetGameInfo> info = game->GetGameInfo();
//
//	boost::shared_ptr<NetPacketSevenCardStudGameListItem> packet(new NetPacketSevenCardStudGameListItem());
//	packet->SetGameID(game->GetID());
//	packet->SetGameType(info->game_type);
//	packet->SetStake(info->stake);
//	packet->SetGameDesc(info->strDescription);
//	packet->SetGameLimit(info->limit_type);
//	packet->SetNumPlayers(game->GetNumPlayers());
//	packet->SetMaxPlayers(info->max_players);
//	packet->SetAntes(info->fAntes);
//	packet->SetBringIn(info->fBringIn);
//	Send( packet, session);
//}
