#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "SFML/Network.hpp" //For sf::Clock


//Used for keeping track of delta time and network ticks
static class TimeManager
{
public:

	TimeManager(int tps=30);

	//To be ran every frame in this order
	void UpdateDeltaTime();
	bool UpdateAndCheckNetworkTickStatus(); //Updates timeThroughTick and returns if tick has been passed (timeThroughTick >= tickSpeed)
	
	float getDeltaTime();

private:
	
	sf::Clock clock;

	static int ticksPerSecond;
	static float tickSpeed;
	static float timeThroughTick; //Used for keeping track of how far through the current tick the game is

	static float deltaTime;
};

#endif