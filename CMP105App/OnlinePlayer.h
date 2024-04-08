#ifndef ONLINEPLAYER_H
#define ONLINEPLAYER_H

#include "Player.h"
#include "NetworkListener.h"
#include <map>

class NetworkManager;

struct OnlinePlayerState {
	sf::Vector2i pos;
	bool crouched;
	int health;
	float stunFramesLeft;

	bool activeLimbs[4];
	bool attacking[4];

	bool operator==(const OnlinePlayerState& other) const {
		return ((pos == other.pos) &&
			    (crouched == other.crouched) &&
				(health == other.health) &&
				(stunFramesLeft == other.stunFramesLeft) &&
				(activeLimbs == other.activeLimbs) &&
			    (attacking == other.attacking));
	}
};

class OnlinePlayer : public Player
{
public:
	OnlinePlayer(float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local);
	void handleInput(float dt, int up, int left, int right, int down, int jab, int kick, int sweep, int upper);
	//void update(float dt);
	/*void SendUpdateDataToNetwork(OnlinePlayerState prevState, OnlinePlayerState newState);*/
	void SendUpdateDataToNetwork(std::vector<int> changedKeys);

	int getPlayerNum();

	void setKeyPressed(int key, bool pressed);

private:
	bool isLocal; //True if this player is controlled by this client, false if controlled by another client
	int playerNum; //e.g. Player 1, Player 2, etc.
	NetworkManager& networkManager; //For sending
	NetworkListener<OnlinePlayer>* networkListener; //For receiving

	OnlinePlayerState prevState;
	OnlinePlayerState newState;

	bool prevKeyState[8];

	std::map<int, bool> keyIsPressed; //For non-local players
};

#endif