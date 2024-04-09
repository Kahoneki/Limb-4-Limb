#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "SFML/Network.hpp" //For sf::Clock


//Used for keeping track of delta time and network ticks
class TimeManager
{
public:

	static TimeManager& getInstance(int tps);

	//To be ran every frame in this order
	void UpdateDeltaTime();
	bool UpdateAndCheckNetworkTickStatus(); //Updates timeThroughTick and returns if tick has been passed (timeThroughTick >= tickSpeed)
	
	float getDeltaTime();

private:
	TimeManager(int tps = 30);

	sf::Clock clock;

	int ticksPerSecond;
	float tickSpeed;
	float timeThroughTick; //Used for keeping track of how far through the current tick the game is

	float deltaTime;
};

#endif