#ifndef ITEMBOX_H
#define ITEMBOX_H

#include "Framework/GameObject.h"


class Player;


enum ItemDrop
{
	START_OF_GOOD_DROPS,
	JumpIncrease,
	SpeedIncrease,
	END_OF_GOOD_DROPS,

	START_OF_BAD_DROPS,
	JumpDecrease,
	SpeedDecrease,
	END_OF_BAD_DROPS
};

struct Effect
{
	ItemDrop itemDrop;
	float riskReward;
};


class ItemBox : public GameObject
{
public:
	ItemBox();
	~ItemBox();
	void ApplyToPlayer(Player& player); //To be called when player collides with item box
	void update(float dt);

private:
	float riskReward; //Between 0 (no risk, no reward) to 1 (full risk, full reward) - this will determine how much impact an item box has - visually represented by the colour of the box
	float chanceOfBeingGood; //Percentage 0:1 - chance of being bad will be 1 - chanceOfBeingGood
	ItemDrop goodDrops[2];
	ItemDrop badDrops[2];
	ItemDrop drop;

	float velocity; //Vertical velocity - item box has no horizontal velocity
};

#endif