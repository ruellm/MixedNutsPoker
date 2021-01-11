#ifndef _DECK_H_
#define _DECK_H_

#include <vector>
#include "Card.h"
#include "Shuffler.h"

class Deck
{
public:
	Deck(void);
	~Deck(void);
	Deck(IShuffler* pShuffler);

	void Shuffle();
	void Reset();
	Card Pop();
	void SetShuffler(IShuffler* pShuffler);
private:
	std::vector<Card> m_CardList;
	IShuffler* m_pShuffler;
	short m_nTopIdx;

	void Fill();
};

#endif