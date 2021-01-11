#ifndef _SHUFFLER_H_
#define _SHUFFLER_H_

#include <vector>
#include <ctime>
#include <boost/random.hpp>
#include "Card.h"

class Random
{
public:
  boost::mt19937 gen;
  boost::uniform_int<int> dst;
  boost::variate_generator< boost::mt19937, boost::uniform_int<int> > rand;
  Random( int N ):// call instance:
    gen( static_cast<unsigned long>(std::time(0)) ), dst( 0, N ), rand( gen, dst ) {
  }
  std::ptrdiff_t operator()( std::ptrdiff_t arg ) {
    return static_cast< std::ptrdiff_t >( rand() );
  }
};

class IShuffler
{
public:
	virtual void Shuffle(std::vector<Card>& deck) = 0;
};

//Standard shuffler
class BoostShuffler: public IShuffler
{
public:
	BoostShuffler();
	virtual void Shuffle(std::vector<Card>& deck);
};

extern Random g_RandSeed;

#endif
