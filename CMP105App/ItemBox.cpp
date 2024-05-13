#include "ItemBox.h"
#include "Player.h"
#include <random>
#include <iostream>


sf::Color LerpColour(sf::Color colourA, sf::Color colourB, float interp);


ItemBox::ItemBox() {

	setSize(sf::Vector2f(30,30));
	setPosition(rand() % (1920 - static_cast<int>(getSize().x)), 0); //Spawn at top of screen in a random horizontal position

	chanceOfBeingGood = 70;
	int riskRewardMultiplier{ 2 }; //Scale the risk-reward by the multiplier
	float randVal { static_cast<float>(rand()) / RAND_MAX};
	riskReward = randVal*riskRewardMultiplier;

	//Green = low risk, low reward. Red = high risk, high reward
	setFillColor(LerpColour(sf::Color::Green, sf::Color::Red, riskReward));

	//Populating goodDrops and badDrops
	for (int indexCount{ 0 }, enumCount{ START_OF_GOOD_DROPS + 1 }; enumCount != END_OF_GOOD_DROPS; ++indexCount, ++enumCount) {
		goodDrops[indexCount] = static_cast<ItemDrop>(enumCount);
	}
	for (int indexCount{ 0 }, enumCount{ START_OF_BAD_DROPS + 1 }; enumCount != END_OF_BAD_DROPS; ++indexCount, ++enumCount) {
		badDrops[indexCount] = static_cast<ItemDrop>(enumCount);
	}

	//Determine if good or bad and choose random drop to assign to item box
	randVal = static_cast<float>(rand()) / RAND_MAX;
	drop = (randVal <= chanceOfBeingGood) ? (goodDrops[rand() % sizeof(goodDrops)/sizeof(goodDrops[0])]) : (badDrops[rand() % sizeof(badDrops) / sizeof(badDrops[0])]);


	velocity = 100.0f;
}

ItemBox::~ItemBox() {}


void ItemBox::ApplyToPlayer(Player& player) {
	
	//Give player the item drop
	switch (drop)
	{
	case JumpIncrease:
		player.setJumpSpeed(player.getJumpSpeed() / (riskReward / 20));
		break;
	case SpeedIncrease:
		player.setTopSpeed(player.getTopSpeed() / (riskReward));
		break;
	case JumpDecrease:
		player.setJumpSpeed(player.getJumpSpeed() * (riskReward / 20));
		break;
	case SpeedDecrease:
		player.setTopSpeed(player.getTopSpeed() * (riskReward));
		break;
	}

	player.setHasEffect(true);
	Effect effect{ drop, riskReward };
	player.setEffect(effect);
}



void ItemBox::update(float dt) {
	setPosition(getPosition().x, getPosition().y + velocity * dt);
}




sf::Color LerpColour(sf::Color a, sf::Color b, float interp) {
	sf::Color returnColour;
	returnColour.r = a.r + (b.r - a.r) * interp;
	returnColour.g = a.g + (b.g - a.g) * interp;
	returnColour.b = a.b + (b.b - a.b) * interp;
	return returnColour;
}