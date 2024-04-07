#ifndef ONLINEPLAYER_H
#define ONLINEPLAYER_H

#include "Player.h"
#include "NetworkListener.h"

class NetworkManager;

struct OnlinePlayerState {
	sf::Vector2f pos;

	bool operator==(const OnlinePlayerState& other) const {
		return (pos == other.pos);
	}
};

class OnlinePlayer : public Player
{
public:
	OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local);
	void update(float dt);
	void SendUpdateDataToNetwork(OnlinePlayerState prevState, OnlinePlayerState newState);

	int getPlayerNum();

private:
	bool isLocal; //True if this player is controlled by this client, false if controlled by another client
	int playerNum; //e.g. Player 1, Player 2, etc.
	NetworkManager& networkManager; //For sending
	NetworkListener<OnlinePlayer>* networkListener; //For receiving
};

#endif