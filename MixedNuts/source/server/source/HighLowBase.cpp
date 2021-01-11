#include <math.h>
#include "HighLowBase.h"
#include "DataManager.h"
#include "ClientSession.h"
#include "PokerHandsLogic.h"

HighLowBase::HighLowBase(void)
{
}

HighLowBase::~HighLowBase(void)
{
}

void HighLowBase::DeclareWinnerHighLow(std::map<IDType,HandStrength> highHands,
							std::map<IDType,HandStrength> lowHands,
							boost::shared_ptr<Table> pGameTable,
							boost::shared_ptr<PacketSenderGame> pSender,
							DataManager< boost::shared_ptr<ClientSession> >&	clientList)
{
	PokerHandsLogic logic;

	//loop thru all pots
	for(unsigned idx=0;idx<pGameTable->GetPotSize();idx++)
	{
		std::vector<HandStrength> playersHigh;
		std::vector<HandStrength> playersLow;
		std::vector<HandStrength> winersHigh;
		std::vector<HandStrength> winersLow;

		Pot* p = pGameTable->GetPotAt(idx);
		
		playersHigh.clear();
		playersLow.clear();
		winersHigh.clear();
		winersLow.clear();

		//get player list from pot
		for(unsigned c=0;c<p->playerList.size();c++){
			HandStrength hands;
			
			//retrieve player from high hand lists
			std::map<IDType, HandStrength>::const_iterator pos = 
				highHands.find(p->playerList[c]->GetID());	//find by key std::map			
			
			if (pos != highHands.end()){
				hands = pos->second;
				playersHigh.push_back(hands);
			}

			//retrieve player from low hand lists
			pos = lowHands.find(p->playerList[c]->GetID());	//find by key std::map			
			if (pos != lowHands.end()){
				hands = pos->second;
				playersLow.push_back(hands);
			}
		}
		
		logic.GetWinningHand(playersHigh,winersHigh);
		if(playersLow.size()>0)
			logic.GetWinningHand(playersLow,winersLow,true);

		float winnerHighPot= 0.0f;
		float winnderLowPot = 0.0f;

		if(winersLow.size() == 0){
			//there is no low pot winner qualified, 
			//divide with high winnders
			winnerHighPot = p->amnt/winersHigh.size();
		}else{
			//Pot will be split into two
			float halfPot = (p->amnt/2.0f);
			winnerHighPot = halfPot/winersHigh.size();
			//get remainder from low and award to hight
			float remainder = fmod(halfPot,(float)winersLow.size());
			winnerHighPot += remainder;
			//set winners low pot
			winnderLowPot = halfPot/winersLow.size();
		}

		//Award winners high
		for(unsigned w=0;w<winersHigh.size();w++){
			Player* player = pGameTable->GetPlayer(winersHigh[w].GetPlayerID());
			if(!player) continue;
			player->SetTotalChips(player->GetTotalChips()+winnerHighPot);
		}

		//Award winners Low
		for(unsigned w=0;w<winersLow.size();w++){
			Player* player = pGameTable->GetPlayer(winersLow[w].GetPlayerID());
			if(!player) continue;
			player->SetTotalChips(player->GetTotalChips()+winnderLowPot);
			
			//send hand for evidence
			pSender->SendHandToAll(player->GetHoleCards(),player->GetID(),clientList);
		}
		
		//notify high winners
		pSender->SendWinnerToAll(winersHigh,idx,winnerHighPot,clientList);
		//notify low winners
		pSender->SendWinnerToAll(winersLow,idx,winnderLowPot,clientList);		
	}
}


////Low Ball Base

void LowBallBase::DeclareWinnerLow(std::map<IDType,HandStrength> playingHands,
						boost::shared_ptr<Table> pGameTable,
						boost::shared_ptr<PacketSenderGame> pSender,
						DataManager< boost::shared_ptr<ClientSession> >&	clientList)
{
	//lowball rules lowest hand wins
	PokerHandsLogic logic;

	//Get winner per pot
	for(unsigned idx=0;idx<pGameTable->GetPotSize();idx++)
	{
		std::vector<HandStrength> players;
		std::vector<HandStrength> winers;
		Pot* p = pGameTable->GetPotAt(idx);
		
		players.clear();
		winers.clear();

		for(unsigned c=0;c<p->playerList.size();c++){
			HandStrength hands;
			std::map<IDType, HandStrength>::const_iterator pos = 
				playingHands.find(p->playerList[c]->GetID());	//find by key std::map
			if (pos != playingHands.end()){
				hands = pos->second;
				players.push_back(hands);
			}
		}

		logic.GetWinningHand(players,winers,true);		
		float pot_total = p->amnt/winers.size();
			
		//award the winner
		for(unsigned w=0;w<winers.size();w++){
			Player* player = pGameTable->GetPlayer(winers[w].GetPlayerID());
			if(!player) continue;
			player->SetTotalChips(player->GetTotalChips()+pot_total);

			//send hand for evidence
			pSender->SendHandToAll(player->GetHoleCards(),player->GetID(),clientList);
		}

		pSender->SendWinnerToAll(winers,idx,pot_total,clientList);
	}
}
