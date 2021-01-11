#ifndef _POKERHANDLOGIC_H_
#define _POKERHANDLOGIC_H_

#include <vector>
#include "Card.h"

#define POKER_HANDS_TOTAL 5

class HandStrength
{
public:

	typedef enum {
		HighCard=0,
		OnePair,
		TwoPair,
		ThreeOfAKind,
		Straight,
		Flush,
		FullHouse,
		FourOfAKind,
		StraightFlush,
		RoyalStraighFlush,
		NoRank = 100,
		LowHand = 101
	}HandRanking;

	HandStrength():m_Rank(NoRank),m_nPlayerID(0){}
	inline void SetPlayerID(unsigned pID){m_nPlayerID=pID; }
	inline unsigned GetPlayerID() const{ return m_nPlayerID; }
	inline HandRanking GetRanking() const{ return m_Rank; }
	inline void SetRanking(HandRanking rank) { m_Rank = rank; }
	
	std::vector<Card> Cards;

protected:
	HandRanking m_Rank;
	unsigned m_nPlayerID;
};

class PokerHandsLogic{
public:
	PokerHandsLogic(){}
	HandStrength CreateHandStrength( const std::vector<Card>& community,
						const std::vector<Card>& hole,
						unsigned playerID = 0);
	HandStrength CreateHandStrength(std::vector<Card>& cards,
						unsigned playerID = 0);

	void GetWinningHand(std::vector<HandStrength>& handList,
		std::vector<HandStrength>& winningHand, bool bLow=false);
	
	bool IsHandXOfAKind(std::vector<Card>& allcards,std::vector<Card>& result,int n);
	bool IsTwoPair(std::vector<Card>& allcards,std::vector<Card>& result);
	bool IsStraight(std::vector<Card>& allcards,std::vector<Card>& result);
	bool IsFlush(std::vector<Card>& allcards,std::vector<Card>& result);
	bool IsFullHouse(std::vector<Card>& allcards,std::vector<Card>& result);
	void GetHandStrength( std::vector<Card>& allcards,
		std::vector<Card>& result,HandStrength::HandRanking* ranking);

	HandStrength* GetWinner(HandStrength* h1, HandStrength* h2);
	HandStrength::HandRanking GetHandRanking(std::vector<Card>& allcards);

	//for low ball and other hi/lo games variant spc 8 or better
	bool IsHandQualifyLow(std::vector<Card>& allcards, std::vector<Card>& result,int low_margin=8);
	HandStrength* GetWinnerLow(HandStrength* h1, HandStrength* h2);

	//Lowball logic
	HandStrength GetLowestHand(std::vector<Card>& allCards, unsigned playerID=0);
	HandStrength CreateHandLowBall(std::vector<Card>& allCards, unsigned playerID=0);
	
private:
	void InsertPair(const Card& card, std::vector<Card>& src, std::vector<Card>& result, int n);
	void SplitAces(std::vector<Card> orig, std::vector<Card>& aces, std::vector<Card>& others);
};



#endif