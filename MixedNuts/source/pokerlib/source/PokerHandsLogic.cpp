#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include <cstring>
#include "PokerHandsLogic.h"

HandStrength PokerHandsLogic::CreateHandStrength( 
		const std::vector<Card>& community,
		const std::vector<Card>& hole,
		unsigned playerID)
{
	HandStrength hs;
	std::vector<Card> allCards;
	HandStrength::HandRanking ranking;

	//crate/merge all cards
	allCards.insert(allCards.end(),community.begin(),community.end());
	allCards.insert(allCards.end(),hole.begin(),hole.end());
	GetHandStrength(allCards,hs.Cards,&ranking);
	hs.SetRanking(ranking);
	hs.SetPlayerID(playerID);

	return hs;
}

HandStrength PokerHandsLogic::CreateHandStrength(std::vector<Card>& cards,
						unsigned playerID)
{
	HandStrength hs;
	HandStrength::HandRanking ranking;

	//crate/merge all cards
	GetHandStrength(cards,hs.Cards,&ranking);
	hs.SetRanking(ranking);
	hs.SetPlayerID(playerID);

	return hs;
}

HandStrength* 
PokerHandsLogic::GetWinner(HandStrength* h1, HandStrength* h2)
{
	HandStrength* pWin = NULL;
	if( h1 == NULL || h2 == NULL ) return NULL;

	if(h1->GetRanking() > h2->GetRanking()) return h1;
	else if(h1->GetRanking() < h2->GetRanking()) return h2;
	else{
		//they are playing with both rank, compare face for kickers
		for(unsigned x=0;x<h1->Cards.size();x++){
			if(h1->Cards[x].GetFace() == h2->Cards[x].GetFace()) continue;
			if(h1->Cards[x].GetFace() > h2->Cards[x].GetFace()){ pWin=h1; break; }
			if(h1->Cards[x].GetFace() < h2->Cards[x].GetFace()){ pWin=h2; break; }
		}
	}
		
	return pWin;
}

void 
PokerHandsLogic::GetHandStrength( std::vector<Card>& allCards,std::vector<Card>& result,HandStrength::HandRanking* ranking)
{
	std::vector<Card> temp;

	std::sort (allCards.begin(), allCards.end(), std::greater<Card>() );

	//clear initial result
	result.clear();	
	
	if( IsStraight(allCards,temp) && IsFlush(allCards,temp)){
		//index 0 is face, index 1 is the suite, no kicker
		if(temp[0].GetFace() == Card::Ace){
			*ranking=HandStrength::RoyalStraighFlush;
		}else{
			*ranking=HandStrength::StraightFlush;
		}		
		result.push_back(temp[0]);
		result.push_back(temp[1]);

	}else if(IsHandXOfAKind(allCards,result,4)){
		*ranking=HandStrength::FourOfAKind;
	}else if(IsFullHouse(allCards,result)){
		*ranking=HandStrength::FullHouse;
	}else if(IsFlush(allCards,result)) {
		*ranking=HandStrength::Flush;
	}else if(IsStraight(allCards,result)) {
		*ranking=HandStrength::Straight;
	}else if(IsHandXOfAKind(allCards,result,3)) {
		*ranking=HandStrength::ThreeOfAKind;
	}else if(IsTwoPair(allCards,result)) {
		*ranking=HandStrength::TwoPair;
	}else if(IsHandXOfAKind(allCards,result,2)){
		*ranking=HandStrength::OnePair;
	}else{
		*ranking= HandStrength::HighCard;		
		result.insert(result.end(),allCards.begin(),allCards.end());
	}	
}

void 
PokerHandsLogic::GetWinningHand(std::vector<HandStrength>& handList,
		std::vector<HandStrength>& winningHand, bool bLow)
{
	int last_wieght = 0;
	HandStrength* pWin=NULL;
	int* weight = new int[handList.size()];
	unsigned x=0;
	typedef HandStrength* (PokerHandsLogic::*POKLOGFNPTR)(HandStrength* h1, HandStrength* h2);
	POKLOGFNPTR fnWinner = (bLow)? &PokerHandsLogic::GetWinnerLow:&PokerHandsLogic::GetWinner;
	
	memset(weight,0,sizeof(int)*handList.size());
	pWin = &handList[x];
	for(x=0;x<handList.size()-1;x++)
	{
		pWin = (this->*fnWinner)(pWin,&handList[x+1]);
		/*if( bLow )
			pWin = GetWinnerLow(pWin,&handList[x+1]);
		else
			pWin = GetWinner(pWin,&handList[x+1]);*/

		if(pWin == NULL) { //split pot
			int value = last_wieght;
			weight[x] = value;
			weight[x+1] = value;
			pWin = &handList[x+1];
			x++;
			continue;
		}

		if(pWin == &handList[x]){
			weight[x] = ++last_wieght;
		}else if(pWin == &handList[x+1]){
			weight[x+1] = ++last_wieght;
		}	
	}
	
	//get winning hands
	for(x=0;x<handList.size();x++){
		if(weight[x]==last_wieght){
			winningHand.push_back(handList[x]);
		}
	}

	delete[] weight;
}
HandStrength::HandRanking 
PokerHandsLogic::GetHandRanking(std::vector<Card>& allcards)
{
	HandStrength::HandRanking  rank;
	std::vector<Card> result;
	GetHandStrength(allcards,result,&rank);
	return rank;
}



bool 
PokerHandsLogic::IsHandXOfAKind(std::vector<Card>& allcards,std::vector<Card>& result,int n)
{
	int ctr=0;
	bool isXOfAKind = false;
	Card::Face face = Card::Joker;
	Card high;
	
	if(allcards.size() <= 0) return false;

	high = allcards.front();
	for(std::vector<Card>::iterator it = allcards.begin();
		it != allcards.end();it++ )
	{
		if( (*it).GetFace() != face )
		{
			face = (*it).GetFace();
			ctr = 1;
			high = (*it);
			continue;
		}
		
		if(++ctr==n) {
			isXOfAKind = true;
			break;
		};
	}
	
	if( isXOfAKind)
	{
		isXOfAKind = true;
		InsertPair(high,allcards,result,n);
		
		//insert kickers
		int ctr_kicker = 0;
		for(std::vector<Card>::iterator it = allcards.begin();
			it != allcards.end();it++){
				if( (*it).GetFace() != face && 
					ctr_kicker++ < (POKER_HANDS_TOTAL-n)){
					result.push_back((*it));
				}
		}
	}

	return isXOfAKind;
}

void PokerHandsLogic::InsertPair(const Card& card,
				std::vector<Card>& src, 
				std::vector<Card>& result, int n)
{
	//insert the pairs
	int ctrPairs=0;
	for(unsigned x=0;x<src.size();x++)
	{
		if( ctrPairs >= n ) break;
		if(card.GetFace() == src[x].GetFace()){
			result.push_back(src[x]);
			ctrPairs++;
		}
	}	
}



bool 
PokerHandsLogic::IsTwoPair(std::vector<Card>& allcards,std::vector<Card>& result)
{
	bool isTwoPair = false;
	
	std::vector<Card> result1;
	std::vector<Card> second_list;

	if(IsHandXOfAKind(allcards,result1,2)){
		Card first= result1.front();
		
		//create another handlist
		for(std::vector<Card>::iterator it = allcards.begin();
			it != allcards.end();it++){
				if( (*it).GetFace() != first.GetFace() ){
					second_list.push_back((*it));
				}
		}
		
		result1.clear();
		if(IsHandXOfAKind(second_list,result1,2)){
			//another pair exist
			Card second = result1.front();			
					
			if(second > first){
				//result.push_back(second);
				//result.push_back(first);
				InsertPair(second,allcards,result,2);
				InsertPair(first,allcards,result,2);
			}else{
				//result.push_back(first);
				//result.push_back(second);
				InsertPair(first,allcards,result,2);
				InsertPair(second,allcards,result,2);
			}
			
			//insert kickers ghapon			
			for(std::vector<Card>::iterator it = allcards.begin();
				it != allcards.end();it++){
					Card cx = *it;
					if(cx.GetFace() != first.GetFace() && 
						cx.GetFace() != second.GetFace())
					{
						result.push_back(cx);
						break;  //for two pair only 1 kicker is valid
					}
			}
			
			isTwoPair = true;
		}
	}

	return isTwoPair;
}

bool 
PokerHandsLogic::IsStraight(std::vector<Card>& allcards,std::vector<Card>& result)
{	
	int ctr=1;
	bool isStraight = false;
	Card high = allcards.front();
	Card::Face last_face = high.GetFace();
	
	//insert initial
	result.push_back(high);

	for(std::vector<Card>::iterator it = allcards.begin();
		it != allcards.end();it++ )
	{	
		//ignore same face
		if(last_face == (*it).GetFace()) continue;
	
		Card::Face nextFace = (Card::Face)(last_face-1);
		if(  nextFace != (*it).GetFace() ){
			ctr=1;
			high = (*it);
			last_face = (*it).GetFace();
			
			//reset list
			result.clear();
			result.push_back((*it));
			continue;
		}
		
		ctr++;
		last_face = (*it).GetFace();
		//insert card as part of the straight
		result.push_back((*it));

		if(ctr==POKER_HANDS_TOTAL){
			isStraight = true;
			break;
		}
	}

	//wheel straight A-5
	if( ctr == POKER_HANDS_TOTAL-1 && 
		last_face == Card::Two &&
		allcards.front().GetFace() == Card::Ace )
	{
		isStraight = true;
		result.push_back(allcards.front());
	}

	if( !isStraight ) {
		result.clear();
	}

	return isStraight;
}

bool 
PokerHandsLogic::IsFlush(std::vector<Card>& allcards,std::vector<Card>& result)
{
	bool isFlush = false;
	int suiteCtr[4]={0,0,0,0};
	Card::Suite suite = Card::None;
	std::vector<Card> temp;

	for(std::vector<Card>::iterator it = allcards.begin();
		it != allcards.end();it++ ){
			if(++suiteCtr[(*it).GetSuite()]==POKER_HANDS_TOTAL){
				suite = (*it).GetSuite();
				isFlush = true;
				break;
			}
	}

	if(isFlush){
		//copy all flush suite
		for(std::vector<Card>::iterator it = allcards.begin();
			it != allcards.end();it++ ){
				if((*it).GetSuite() == suite){
					result.push_back((*it));
				}
		}

		//sort to put the highest card on top
		std::sort (result.begin(), result.end(), std::greater<Card>() );

		//just insert the highest card
		/*Card card;
		card.SetSuite(temp[0].GetSuite());
		card.SetFace(temp[0].GetFace());
		result.push_back(card);*/
	}

	return isFlush;
}

bool 
PokerHandsLogic::IsFullHouse(std::vector<Card>& allcards,std::vector<Card>& result)
{
	bool isFullHouse = false;
	std::vector<Card> result1;
	std::vector<Card> second_list;

	//contains 3 of a kind
	if(IsHandXOfAKind(allcards,result1,3)){
		Card tripair = result1.front();
		
		//create another handlist
		for(std::vector<Card>::iterator it = allcards.begin();
			it != allcards.end();it++){
				if( (*it).GetFace() != tripair.GetFace() ){
					second_list.push_back((*it));
				}
		}

		//contains 2 pairs
		result1.clear();
		if(IsHandXOfAKind(second_list,result1,2))
		{
			Card twopair = result1.front();
			/*result.push_back(tripair);
			result.push_back(twopair);*/
			InsertPair(tripair,allcards,result,3);
			InsertPair(twopair,allcards,result,2);
			isFullHouse=true;
		}
	}

	return isFullHouse;
}

//Poker hands to qualify lowbased games
//-hands must not have a pair
//-hands must be less than low_margin, common is 8
//-Ace are considered as 1(lowest)
//-flush and straight do not count
//-Low hands are counted from the top down, 
//meaning the hand is only as good as its highest card
bool 
PokerHandsLogic::IsHandQualifyLow(std::vector<Card>& allCards, 
								   std::vector<Card>& result,
								   int low_margin)
{
	int count_valid=0;
	const int eight = 8;
	std::vector<Card> lessThan;
	std::vector<Card> final_hand;
	bool bFoundAce= false;

	//extract less than 8 face, including ace(as 1)
	for(unsigned x=0;x<allCards.size();x++){
		if( allCards[x].GetFace() <= low_margin ||
			allCards[x].GetFace() == Card::Ace){
				lessThan.push_back(allCards[x]);
		}
	}
	
	if( lessThan.size() < POKER_HANDS_TOTAL )
		return false;

 	HandStrength lowest = GetLowestHand(lessThan);
	if( lowest.GetRanking() != HandStrength::HighCard )
		return false;
	
	result.clear();
	result.insert(result.end(),
		lowest.Cards.begin(),lowest.Cards.end());

	return true;
}

HandStrength* 
PokerHandsLogic::GetWinnerLow(HandStrength* h1, 
								   HandStrength* h2)
{
	HandStrength* pWin = NULL;
	if( h1 == NULL || h2 == NULL ) return NULL;

	if(h1->GetRanking() < h2->GetRanking()) return h1;
	else if(h1->GetRanking() > h2->GetRanking()) return h2;
	else{
		//they are playing with both rank, compare face for kickers
		for(unsigned x=0;x<h1->Cards.size();x++){
			if( h1->Cards[x].GetFace() == Card::Ace && 
				h2->Cards[x].GetFace() != Card::Ace) {
				pWin = h1; 
				break;
			}else if( h2->Cards[x].GetFace() == Card::Ace && 
				h1->Cards[x].GetFace() != Card::Ace)
			{
				pWin = h2;
				break;
			}else{
				if( h1->Cards[x].GetFace() == h2->Cards[x].GetFace()) continue;
				if(h1->Cards[x].GetFace() > h2->Cards[x].GetFace()){ pWin=h2; break; }
				if(h1->Cards[x].GetFace() < h2->Cards[x].GetFace()){ pWin=h1; break; }
			}		
		}
	}
	return pWin;
}



//low ball -added 03/19/2010

HandStrength 
PokerHandsLogic::CreateHandLowBall(std::vector<Card>& allCards, 
				  unsigned playerID)
{
	HandStrength hs;
	HandStrength::HandRanking ranking;
	std::vector<Card> temp;	
	hs.SetPlayerID(playerID);		

	std::sort (allCards.begin(), allCards.end(), std::greater<Card>() );
	
	if(IsHandXOfAKind(allCards,hs.Cards,4)){
		ranking=HandStrength::FourOfAKind;
	}else if(IsFullHouse(allCards,hs.Cards)){
		ranking=HandStrength::FullHouse;
	}else if(IsHandXOfAKind(allCards,hs.Cards,3)) {
		ranking=HandStrength::ThreeOfAKind;
	}else if(IsTwoPair(allCards,hs.Cards)) {
		ranking=HandStrength::TwoPair;
	}else if(IsHandXOfAKind(allCards,hs.Cards,2)){
		ranking=HandStrength::OnePair;
	}else{
		ranking= HandStrength::HighCard;				
		
		//extract all the aces		
		std::vector<Card> aces;
		std::vector<Card> others;
		SplitAces(allCards,aces,others);

		//insert other cards
		hs.Cards.insert(hs.Cards.end(),others.begin(),others.end());
		//insert aces at the back
		hs.Cards.insert(hs.Cards.end(),aces.begin(),aces.end());

	}	
	hs.SetRanking (ranking);
	return hs;
}

HandStrength 
PokerHandsLogic::GetLowestHand(std::vector<Card>& hands, 
							   unsigned playerID)
{
	std::vector< std::vector<Card> >	combo;		
	HandStrength lowest;
	
	if(hands.size() <=  POKER_HANDS_TOTAL){
		lowest = CreateHandLowBall(hands);
	}else if( hands.size() >  POKER_HANDS_TOTAL) {
		combo = GroupCardByFive(hands);
		lowest = CreateHandLowBall(combo[0]);
		for(unsigned x=1;x<combo.size();x++){
			HandStrength currentHand = 
					CreateHandLowBall(combo[x]);		
			HandStrength* pWinner =	
				GetWinnerLow(&currentHand,&lowest);
			if( pWinner == &currentHand ){
				lowest = currentHand;
			}	
		}
	}
	
	lowest.SetPlayerID(playerID);
	return lowest;
}

void 
PokerHandsLogic::SplitAces(std::vector<Card> orig, 
						   std::vector<Card>& aces, 
						   std::vector<Card>& others)
{
	for(unsigned x=0;x<orig.size();x++){
		if(orig[x].GetFace() == Card::Ace)
			aces.push_back(orig[x]);
		else
			others.push_back(orig[x]);
	}
}
