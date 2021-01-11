#include "Card.h"

std::vector< std::vector<Card> >	
GroupCardByFive(std::vector<Card>& card_list)
{
	std::vector< std::vector<Card> > sets;
	for(unsigned x=0;x<card_list.size()-4;x++)
	{
		for(unsigned y=x+1;y<card_list.size()-3;y++)
		{
			for(unsigned z=y+1;z<card_list.size()-2;z++)
			{
				for(unsigned a=z+1;a<card_list.size()-1;a++)
				{
					for(unsigned b=a+1;b<card_list.size();b++)
					{
						std::vector<Card> new_set;
						new_set.push_back(card_list[x]);
						new_set.push_back(card_list[y]);
						new_set.push_back(card_list[z]);
						new_set.push_back(card_list[a]);
						new_set.push_back(card_list[b]);
						sets.push_back(new_set);
					}
				}				
			}
		}
	}
	return sets;
}

std::vector< std::vector<Card> >
		GroupCardByTwo(std::vector<Card>& card_list)
{
	std::vector< std::vector<Card> > sets;
	for(unsigned x=0;x<card_list.size()-1;x++)
	{
		for(unsigned y=x+1;y<card_list.size();y++)
		{
			std::vector<Card> new_set;
			new_set.push_back(card_list[x]);
			new_set.push_back(card_list[y]);
			sets.push_back(new_set);
		}
	}
	return sets;
}

std::vector< std::vector<Card> >
	GroupCardByThree(std::vector<Card>& card_list)
{
	std::vector< std::vector<Card> > sets;
	for(unsigned x=0;x<card_list.size()-2;x++)
	{
		for(unsigned y=x+1;y<card_list.size()-1;y++)
		{
			for(unsigned z=y+1;z<card_list.size();z++)
			{
				std::vector<Card> new_set;
				new_set.push_back(card_list[x]);
				new_set.push_back(card_list[y]);
				new_set.push_back(card_list[z]);
				sets.push_back(new_set);
			}
		}
	}
	return sets;
}
