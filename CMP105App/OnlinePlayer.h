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
	OnlinePlayer(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip, int pn, bool local);
	void handleInput(float dt, int jump, int left, int right, int down, int dodge, int jab, int kick, int sweep, int upper);
	void update(float dt);
	void SendUpdateDataToNetwork(std::vector<int> changedKeys);
	void SendUpdateDataToNetwork(sf::Vector2f newPosition);

	int getPlayerNum();

	friend class NetworkListener<OnlinePlayer>;

private:
	bool isLocal; //True if this player is controlled by this client, false if controlled by another client
	int playerNum; //e.g. Player 1, Player 2, etc.
	
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