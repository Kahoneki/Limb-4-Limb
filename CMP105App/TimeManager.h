#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "SFML/Network.hpp" //For sf::Clock


//Used for keeping track of delta time and network ticks
class TimeManager
{
public:

	static TimeManager& getInstance(int tps);

	//To be ran every frame in this order
	bool UpdateDeltaTimeAndCheckForNewFrame();
	bool UpdateAndCheckNetworkTickStatus(); //Updates timeThroughTick and returns if tick has been passed (timeThroughTick >= tickSpeed)
	
	float getDeltaTime();

	static constexpr int PhysicsClockFramerate{ 30 };

private:
	TimeManager(int tps = 240);

	sf::Clock clock;

	float timeThroughFrame;
	float frameSpeed;

	int ticksPerSecond;
	float tickSpeed;
	float timeThroughTick; //Used for keeping track of how far through the current tick the game is

	float deltaTime;
};

#endif