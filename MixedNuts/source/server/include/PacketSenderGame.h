#ifndef _PACKETSENDERGAME_H_
#define _PACKETSENDERGAME_H_

#include <vector>
#include "PacketSender.h"
#include "Card.h"
#include "Table.h"
#include "DataManager.h"
#include "PokerHandsLogic.h"
#include "NetPacketGame.h"
#include "Player.h"

class PacketSenderGame: public PacketSender
{
public:
	PacketSenderGame(void);
	PacketSenderGame(IDType id);
	~PacketSenderGame(void);
	void SetRoomID(IDType id);

	void SendHoleCard( const std::vector<Card>& hole,
		boost::shared_ptr<ClientSession> session);

	void SendCards( GamePacket id, const std::vector<Card>& cards,
		boost::shared_ptr<ClientSession> session);
	
	void SendFlop( const std::vector<Card>& cards,
		DataManager< boost::shared_ptr<ClientSession> >& clientList);
	
	void SendCard( GamePacket id, const Card& card, 
		DataManager< boost::shared_ptr<ClientSession> >& clientList);

	//void SendTableHoldemSnapShot(boost::shared_ptr<TableHoldem> table,
	//	boost::shared_ptr<ClientSession> session);

	/*void SendAllTableHoldemSnapShot(boost::shared_ptr<TableHoldem> table,
		DataManager<boost::shared_ptr<ClientSession>>& clientList);*/

	void SendAllTablePokerSnapShot(boost::shared_ptr<Table> table,
		DataManager< boost::shared_ptr<ClientSession> >& clientList);

	void SendWaitAction(boost::shared_ptr<ClientSession> session);

	void SendPacketToAll(boost::shared_ptr<INetPacket> packet, 
		DataManager< boost::shared_ptr<ClientSession> >& clientList);
	
	void SendWinnerToAll(std::vector<HandStrength>& winers, short potnum,float amnt,
		DataManager< boost::shared_ptr<ClientSession> >& clientList);

	void SendHandToAll(std::vector<Card>& hands,IDType id,
		DataManager< boost::shared_ptr<ClientSession> >& clientList);
	
	void SendMsgToAll(const std::string& msg, IDType pID,
		DataManager< boost::shared_ptr<ClientSession> >& clientList);
	
	void SendPlayerList(std::vector<Player*>& playerList,
		boost::shared_ptr<ClientSession> session);

	void SendOpenCard(std::vector<CardOwn>& ownerList,GamePacket id,
		DataManager< boost::shared_ptr<ClientSession> >& clientList);

	void SendAllRoomParam(GamePacket id, int param, 
				DataManager< boost::shared_ptr<ClientSession> >& clientList);
	
	void SendGameInfo(boost::shared_ptr<GameBase> game,
		boost::shared_ptr<ClientSession> session);

	void SendPokerGameInfo(boost::shared_ptr<PokerGameBase> base,
		boost::shared_ptr<ClientSession> session);

	//void SendSevenCardStudGameInfo(boost::shared_ptr<PokerGameBase> base,
	//	boost::shared_ptr<ClientSession> session);

private: 
	IDType m_RoomID;
};

#endif
