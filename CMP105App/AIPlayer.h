#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "Player.h"


class AIPlayer : public Player
{
public:
	AIPlayer(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip, int difficultyLevel);

	void handleInput(float dt, int jump, int left, int right, int down, int dodge, Player& opponent, sf::FloatRect ground);

private:
	std::map<int, bool> keyIsPressed;

	int difficulty;
	int minDifficultyLevel;
	int maxDifficultyLevel;
	float flipChance; //The higher the difficulty, the lower the flip chance. This is the chance (0:1) of a keyIsPressed value flipping state

	bool withinAttackingRange;
};


#endif