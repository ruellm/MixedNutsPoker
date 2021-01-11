#include "Deck.h"

Deck::Deck(void):
m_nTopIdx(0)
{
	Fill();
}

Deck::~Deck(void)
{
	if(m_pShuffler){
		delete m_pShuffler;
		m_pShuffler = NULL;
	}
}

Deck::Deck(IShuffler* pShuffler):
	m_pShuffler(pShuffler)
{
	
}

void Deck::Shuffle()
{
	if(!m_pShuffler) return;
	m_pShuffler->Shuffle(m_CardList);
}

void Deck::Reset()
{
	m_nTopIdx=0;
	Shuffle();
}

Card 
Deck::Pop()
{	
	if(m_nTopIdx>=m_CardList.size()) {
		Card card;	
		card.SetSuite(Card::None);
		card.SetFace(Card::Joker);
		return card;
	}
	return m_CardList[m_nTopIdx++];
}

void Deck::Fill()
{
	int suite=0;
	int face=0;
	const int num_suite = 4;
	const int num_face = 14;

	for(suite=0;suite<num_suite;suite++){
		for(face=2;face<=num_face;face++){
			Card card;
			card.SetSuite((Card::Suite)suite);
			card.SetFace((Card::Face)face);
			m_CardList.push_back(card);
		}
	}
}

void Deck::SetShuffler(IShuffler* pShuffler)
{
	m_pShuffler=pShuffler;
}
