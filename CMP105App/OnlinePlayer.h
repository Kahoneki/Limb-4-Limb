#ifndef ONLINEPLAYER_H
#define ONLINEPLAYER_H

#include "Player.h"
#include "NetworkListener.h"

class NetworkManager;

class OnlinePlayer : public Player
{
public:
	OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn);
	void update(float dt);
	void SendUpdateDataToNetwork(sf::Vector2f prevPosition);

	int getPlayerNum();

private:
	int playerNum; //e.g. Player 1, Player 2, etc.
	NetworkManager& networkManager; //For sending
	NetworkListener<OnlinePlayer>* networkListener; //For receiving
};

#endif