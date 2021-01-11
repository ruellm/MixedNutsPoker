#ifndef _CARD_H_
#define _CARD_H_

//card definition

class Card{
public:	

	typedef enum {
		Clubs = 0,
		Diamond  = 1,
		Heart = 2,
		Spade = 3,
		None = 100
	}Suite;

	typedef enum {
		Ace = 14,
		Two = 2,
		Three = 3, 
		Four = 4, 
		Five = 5, 
		Six = 6, 
		Seven = 7,
		Eight = 8, 
		Nine = 9,
		Ten = 10, 
		Jack = 11,
		Queen = 12,
		King = 13,
		Joker = 100
	}Face;


	Card(){}
	Card(Face f, Suite s): m_Suite(s),m_Face(f){}
	
	inline Face GetFace() const{ return m_Face; } 
	inline Suite GetSuite() const{ return m_Suite; } 
	inline void SetFace( Face f )	{ m_Face=f; }
	inline void SetSuite( Suite s) { m_Suite=s; }

	bool operator > (const Card& c) const { 
		return ( (int)GetFace() > (int)c.GetFace());	
	}
	bool operator < (const Card& c) const { 
		return (GetFace() < c.GetFace());
	
	}
	bool operator == (const Card& c) const { 
		return (GetFace() == c.GetFace());	
	}

private:
	Suite m_Suite;
	Face m_Face;
};

#include <vector>
#include "definitions.h"

//Helper structure
typedef struct{
	IDType playerID;
	std::vector<Card> hands;
}CardOwn;


//card helper prototypes
extern std::vector< std::vector<Card> >
	GroupCardByFive(std::vector<Card>& card_list);

extern std::vector< std::vector<Card> >
		GroupCardByTwo(std::vector<Card>& card_list);

extern std::vector< std::vector<Card> >
		GroupCardByThree(std::vector<Card>& card_list);

#endif
