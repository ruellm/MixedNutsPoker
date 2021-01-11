#ifndef _TABLE_H_
#define _TABLE_H_

#include <vector>
#include "Player.h"

typedef struct
{
	bool occupied;
	//unsigned short seat_number;	//necessary?
	Player* player;
}Seat;

typedef struct{
	float currRaise;
	float amnt;
	Player* pRaiser;
	std::vector<Player*> playerList;
	bool final;
}Pot;

//TODO: some of these functionalities are exclusively for poker only
//need refactoring
class Table
{
public:
	void Build(int numSeats);
	Player* GetPlayerAt(short seatno);
	Player* GetPlayer(IDType id);
	void SeatDownPlayer(short seatno, Player* player);
	void RemovePlayer(short seatno);
	bool IsSeatVacant(short seatno);
	int GetDealer()const;
	void SetDealer(short idx);
	bool IsSeatPlaying(short seatno);	
	short GetSeatCount() const;
	short GetCurrentBettor() const;
	void SetCurrentBettor(unsigned short idx);
	short GetRoundPlayerCount();
	bool IsBettingDone();
	bool DidPlayersFold();
	bool CheckAllIn(bool newRound);
	bool IfSetValid(short seatno);
	void ResetPlayers();
	Pot* GetCurrentPot();
	bool IfSeatIsPlayingRound(short seat);
	short CountIsRoundPlayers();

	void RemovePlayerFromPot(Player* p,Pot* pot=NULL);
	void RemovePlayerFromAllPot(Player*p);
	void PostBet(float amnt, Player* p);

	bool IfSetCanPlay(short seatno);
	void CollectBets();	
	void ResetTable();

	short FindNextValid(short seatno);	
	
	void SetState(TableState state);
	TableState GetState() const;

	void StandUpPlayer(IDType id);
	size_t GetPotSize() const;
	Pot* GetPotAt(int ix);

	short GetReadyState();

protected:	
	std::vector<Seat> m_Seats;
	short m_iDealerIndex;	
	short m_usCurrentBettor;
	short m_nRoundPlayerCnt;
	std::vector<Pot> m_PotList;
	TableState   m_State;

private:
	bool DidEveryOneCheck();
	void AddNewPot();
};

//class TableHoldem :
//	public Table
//{
//public:
//	TableHoldem():m_nSBIdx(0),m_nBBIdx(0)
//	{}
//
//	inline void SetBBIndex(short idx){
//		m_nBBIdx = idx;
//	}
//	
//	inline void SetSBIndex(short idx){
//		m_nSBIdx = idx;
//	}
//
//	inline short GetBBIndex() const{
//		return m_nBBIdx;
//	}
//
//	inline short GetSBIndex() const{
//		return m_nSBIdx;
//	}
//
//private:
//	short m_nBBIdx;
//	short m_nSBIdx;
//
//};

#endif
