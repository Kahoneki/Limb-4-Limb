#include "TimeManager.h"

TimeManager& TimeManager::getInstance(int tps = 30) {
	static TimeManager instance(tps);
	return instance;
}

TimeManager::TimeManager(int tps) {
	ticksPerSecond = tps;
	tickSpeed = 1 / ticksPerSecond;
	timeThroughTick = 0;

	deltaTime = 0;
}

void TimeManager::UpdateDeltaTime() {
	deltaTime = clock.restart().asSeconds();
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
