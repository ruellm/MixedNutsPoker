#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include <string>
#include "definitions.h"
#include "Card.h"

//TODO: some of these functionalities are exclusively for poker only
//need refactoring
class Player
{
public:
	Player(IDType id);
	~Player(void);
	IDType GetID() const;
	
	void SetTotalChips(float amnt);
	void SetIsPlaying(bool value);
	float GetTotalChips() const;
	float GetBet() const;
	void SetBet(float bet);
	bool IsPlaying() const;
	void AddHoleCard(Card& card);
	std::vector<Card>& GetHoleCards();
	void SetAction(PokerAction action);
	PokerAction GetAction( ) const;
	void SetSeatNum(short seatno);
	short GetSeatNum() const;
	void ResetHoleCards();
	bool IsInRound();
	void SetInRound(bool value);

	/*inline void SetPlayerName(std::string name){
		m_strPlayerName = name;
	}
	inline void SetLocation(std::string location){
		m_strLocation = location;
	}

	inline std::string GetPlayerName() const{
		return m_strPlayerName;
	}

	inline std::string GetLocation() const{
		return m_strLocation;
	}*/

private:
	SessionIDType m_nID;				//Maps to seassion ID
	std::vector<Card> m_HoleCards;
	float m_fBet;				//current stake/bet
	float m_fTotalChips;			//Player's buy-in chips
	bool m_bIsPlaying;     //TBD: if found in player
	bool m_bIsShowCards;   //TBD: if found in player	
	//bool m_bIsAllIn;		//TBD: if found in player
	bool m_bInRound;		//if player is in current round
	PokerAction m_Action;	//player latest action
	short m_nSeatNum;

	//TBD: if necessary
	/*std::string m_strPlayerName;
	std::string m_strLocation;*/
	//action next and last
	//current stake/bet
	//player action class
};

#endif