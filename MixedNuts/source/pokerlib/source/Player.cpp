#include "Player.h"

Player::Player(IDType id):
	m_nID(id),
	m_fBet(0.0),m_fTotalChips(0.0),	m_bIsPlaying(false),  
	m_bIsShowCards(false),
	m_Action(eIdle),m_bInRound(false)
{
}

Player::~Player(void)
{
}

IDType 
Player::GetID() const
{ return m_nID; }

void Player::SetIsPlaying(bool value)
{ m_bIsPlaying=value; }

void 
Player::SetTotalChips(float amnt)
{ m_fTotalChips = amnt; }

float 
Player::GetTotalChips() const
{ return m_fTotalChips; }

bool 
Player::IsPlaying() const
{ return m_bIsPlaying; }

void Player::AddHoleCard(Card& card)
{
	m_HoleCards.push_back(card);
}

void 
Player::ResetHoleCards()
{ m_HoleCards.clear(); }

std::vector<Card>& 
Player::GetHoleCards()
{
	return m_HoleCards;
}

float Player::GetBet() const
{ return m_fBet;}

void Player::SetBet(float bet)
{ 
	m_fBet = bet; 
}

void Player::SetAction(PokerAction action)
{ m_Action = action; }

PokerAction 
Player::GetAction( ) const
{ return m_Action;}

void Player::SetSeatNum(short seatno)
{ m_nSeatNum = seatno; }
short Player::GetSeatNum() const
{ return m_nSeatNum; }

bool Player::IsInRound()
{ return m_bInRound; }

void Player::SetInRound(bool value)
{ m_bInRound = value; }
