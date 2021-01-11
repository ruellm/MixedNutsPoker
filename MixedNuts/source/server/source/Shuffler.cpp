#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <algorithm>
#include "Shuffler.h"

using namespace std;

//intitiazes random generator
Random g_RandSeed(52);

//Standard shuffler
BoostShuffler::BoostShuffler()
{	
}

void BoostShuffler::Shuffle(std::vector<Card>& deck)
{		

	// using built-in random generator:
	random_shuffle (deck.begin(), deck.end(),g_RandSeed);

	//try reversing the cards
	reverse(deck.begin(),deck.end()); 

	//randomize again
	random_shuffle (deck.begin(), deck.end(),g_RandSeed);
}

