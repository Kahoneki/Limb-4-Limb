#include "ItemBox.h"
#include "Player.h"
#include <random>
#include <iostream>


sf::Color LerpColour(sf::Color colourA, sf::Color colourB, float interp);


ItemBox::ItemBox() {

	setSize(sf::Vector2f(30, 30));
	setPosition(rand() % (1920 - static_cast<int>(getSize().x)), 0); //Spawn at top of screen in a random horizontal position

	chanceOfBeingGood = 0.65f;
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



ItemBox::ItemBox(float startX, ItemDrop itemDrop, float rr) {
	setSize(sf::Vector2f(30, 30));
	setPosition(startX, 0);

	riskReward = rr;
	//Green = low risk, low reward. Red = high risk, high reward
	setFillColor(LerpColour(sf::Color::Green, sf::Color::Red, riskReward));

	drop = itemDrop;

	velocity = 100.0f;
}



void ItemBox::ApplyToPlayer(Player& player) {
	
	//Give player the item drop
	switch (drop)
	{
	case JumpIncrease:
		player.setJumpSpeed(player.getJumpSpeed() + player.getJumpSpeed() * (riskReward / 3));
		break;
	case SpeedIncrease:
		player.setTopSpeed(player.getTopSpeed() + player.getTopSpeed() * (riskReward / 2));;
		break;
	case HealthIncrease:
		player.setHealth(player.getHealth() + player.getHealth()*riskReward);
		if (player.getHealth() > player.getMaxHealth()) { player.setHealth(player.getMaxHealth()); }
		break;
	case MaxHealthIncrease:
		player.setMaxHealth(player.getMaxHealth() + player.getMaxHealth() * (riskReward / 2));
		player.setHealth(player.getMaxHealth());
		break;
	case Invincibility:
		player.setInvincible(true);
		break;
	case JumpDecrease:
		player.setJumpSpeed(player.getJumpSpeed() - player.getJumpSpeed() * (riskReward / 3));
		break;
	case SpeedDecrease:
		player.setTopSpeed(player.getTopSpeed() - player.getTopSpeed() * (riskReward / 2));
		break;
	case HealthDecrease:
		player.setHealth(player.getHealth() - player.getHealth()*riskReward);
		if (player.getHealth() <= 0) { player.setHealth(1); } //because im nice
		break;
	case MaxHealthDecrease:
		player.setMaxHealth(player.getMaxHealth() - player.getMaxHealth() * (riskReward / 2));
		if (player.getHealth() > player.getMaxHealth()) { player.setHealth(player.getMaxHealth()); } //because im evil
		break;
	case FlippedControls:
		player.setUseFlippedControls(true);
		break;
	}

	player.setHasEffect(true);
	Effect effect{ drop, riskReward };
	player.setEffect(effect);
	std::cout << "drop: " << drop << '\n';
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