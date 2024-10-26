#include "TimeManager.h"

TimeManager& TimeManager::getInstance(int tps=240) {
	static TimeManager instance(tps);
	return instance;
}

TimeManager::TimeManager(int tps) {
	frameSpeed = 1 / PhysicsClockFramerate;
	timeThroughFrame = 0;

	ticksPerSecond = tps;
	tickSpeed = 1 / ticksPerSecond;
	timeThroughTick = 0;
	
	deltaTime = 0;
}

bool TimeManager::UpdateDeltaTimeAndCheckForNewFrame() {
	deltaTime = clock.restart().asSeconds();
	timeThroughFrame += deltaTime;
	if (timeThroughFrame >= frameSpeed) {
		timeThroughFrame = 0;
		return true;
	}
	return false;
}

bool TimeManager::UpdateAndCheckNetworkTickStatus() {
	timeThroughTick += deltaTime;
	if (timeThroughTick >= tickSpeed) {
		timeThroughTick = 0;
		return true;
	}
	return false;
}

float TimeManager::getDeltaTime() {
	return deltaTime;
}
