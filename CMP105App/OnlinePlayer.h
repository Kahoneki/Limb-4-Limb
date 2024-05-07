#ifndef ONLINEPLAYER_H
#define ONLINEPLAYER_H

#include "Player.h"
#include "TimeManager.h"
#include <map>

class NetworkManager;

template<typename ParentType>
class NetworkListener;

class OnlinePlayer : public Player
{
public:
	OnlinePlayer(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local, int nmi);
	void handleInput(float dt, int jump, int left, int right, int down, int dodge, int jab, int kick, int sweep, int upper);
	void update(float dt);
	void SendUpdateDataToNetwork(std::vector<int> changedKeys);
	void SendUpdateDataToNetwork(sf::Vector2f newPosition);

	int getPlayerNum();

	friend class NetworkListener<OnlinePlayer>;

private:
	int playerNum; //e.g. Player 1, Player 2, etc.
	bool isLocal; //True if this player is controlled by this client, false if controlled by another client
	int opponentNetworkManagerIndex; //The nmi associated with this player's opponent (the nmi that local data will be sent to) - WILL BE -1 IF PLAYER ISN'T LOCAL (since online players shouldn't send data locally)
	int networkListenerIndex; //The nli associated with this player
	
	TimeManager& timeManager;
	float verificationPacketTimer; //For keeping track of how long its been since last verification packet has been sent
	float verificationPacketCooldown; //Verification packets should be sent once every verificationPacketCooldown seconds

	NetworkManager& networkManager; //For sending
	NetworkListener<OnlinePlayer>* networkListener; //For receiving

	bool prevKeyState[6];

	sf::Vector2f prevPosition;

	std::map<int, bool> keyIsPressed; //For non-local players
};

#endif