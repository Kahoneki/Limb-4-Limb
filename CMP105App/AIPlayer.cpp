#include "AIPlayer.h"
#include "TimeManager.h"
#include <iostream>
#include <random>

AIPlayer::AIPlayer(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip, int difficultyLevel)
	: Player(size, acc, ts, js, hp, prot, c1, flip)
{

	minDifficultyLevel = 1;
	maxDifficultyLevel = 5;

	difficulty = difficultyLevel;
	//Clamp difficulty
	if (difficulty < minDifficultyLevel) { difficulty = minDifficultyLevel; }
	else if (difficulty > maxDifficultyLevel) { difficulty = maxDifficultyLevel; }

	std::map<int, float> difficultyToFlipChance;
	difficultyToFlipChance[1] = 0.05f;
	difficultyToFlipChance[2] = 0.04f;
	difficultyToFlipChance[3] = 0.03f;
	difficultyToFlipChance[4] = 0.015f;
	difficultyToFlipChance[5] = 0.0f;
	flipChance = difficultyToFlipChance[difficulty];

	withinAttackingRange = false;
}


void AIPlayer::handleInput(float dt, int jump, int left, int right, int down, int dodge, Player& opponent, sf::FloatRect ground)
{

	keyIsPressed[jump] = false;
	keyIsPressed[left] = false;
	keyIsPressed[right] = false;
	keyIsPressed[down] = false;
	keyIsPressed[dodge] = false;

	if (dodgeFramesLeft) { actionable = false; }

	//If player is within attacking range, is actionable, and is not crouching, use attack (if multiple possibilities, choose one at random)
	if (actionable && !crouched) {
		std::vector<Attack*> possibleAttacks;

		for (Attack& atk : attacks) {
			sf::RectangleShape hitboxShape{};
			hitboxShape.setSize(sf::Vector2f(atk.width, atk.height));
			if (flipped) {
				hitboxShape.setPosition(sf::Vector2f(getPosition().x - atk.relative_xpos - atk.width, getPosition().y + atk.relative_ypos));
			}
			else {
				hitboxShape.setPosition(sf::Vector2f(getPosition().x + atk.relative_xpos - atk.width, getPosition().y + atk.relative_ypos));
			}

			if (hitboxShape.getGlobalBounds().intersects(opponent.getEffectiveCollider())) {
				possibleAttacks.push_back(&atk);
			}
		}

		if (!possibleAttacks.empty()) {
			withinAttackingRange = true;
			Attack* randAttack{ possibleAttacks[rand() % possibleAttacks.size()] };
			randAttack->setAttacking(true);
		}
		else {
			withinAttackingRange = false;
		}
	}


	//None of player's attacks are in range, move towards them
	
	float epsilon{ 1.0f };

	//If player is lower down than opponent (and both players are grounded), jump
	if ((getPosition().y > (opponent.getPosition().y + epsilon)) && opponent.getGrounded()) {
		keyIsPressed[jump] = true;
	}

	//If player is higher up than opponent (and both players are grounded), crouch
	else if ((getPosition().y < (opponent.getPosition().y - epsilon)) && opponent.getGrounded()) {
		keyIsPressed[down] = true;
	}


	if (difficulty == 5) {
		//Perfect movement, always move towards player no matter what
		if (getPosition().x < opponent.getPosition().x) {
			keyIsPressed[right] = true;
		}
		else if (getPosition().x > opponent.getPosition().x) {
			keyIsPressed[left] = true;
		}
	}

	else {
		//Only move towards player if not within attacking range
		if (!withinAttackingRange) {
			//90% chance to move player horizontally towards opponent if opponent is approaching player, otherwise have a 50% chance of running towards the player
			if (getPosition().x < opponent.getPosition().x) {
				if ((opponent.getVelocity().x > 0 && opponent.getPosition().x > getPosition().x) || ((opponent.getVelocity().x < 0 && opponent.getPosition().x < getPosition().x))) {
					keyIsPressed[right] = (static_cast<double>(rand())/RAND_MAX) <= 90;
				}
				else {
					keyIsPressed[right] = (static_cast<double>(rand()) / RAND_MAX) <= 50;
				}
			}
			else if (getPosition().x > opponent.getPosition().x) {
				if ((opponent.getVelocity().x > 0 && opponent.getPosition().x < getPosition().x) || ((opponent.getVelocity().x < 0 && opponent.getPosition().x < getPosition().x))) {
					keyIsPressed[left] = (static_cast<double>(rand()) / RAND_MAX) <= 90;
				}
				else {
					keyIsPressed[left] = (static_cast<double>(rand()) / RAND_MAX) <= 50;
				}
			}
		}
	}

	//Overwrite all the above direction information if player is over the edge
	if (getPosition().y < ground.left) {
		keyIsPressed[right] = true;
		keyIsPressed[left] = false;
	}
	else if (getPosition().y > ground.left + ground.width) {
		keyIsPressed[left] = true;
		keyIsPressed[right] = false;
	}

	if (difficulty == 4 || difficulty == 5) {
		//These difficulty levels have the capacity to dodge to get closer to the player or to get back on the ground if they've fallen off the edge
		bool offEdge{ (getPosition().y < ground.left) || (getPosition().y > ground.left + ground.width) };
		float horizontalDistanceToPlayer{ abs(getPosition().x - opponent.getPosition().x) };
		bool dashTowardsPlayer{ horizontalDistanceToPlayer > ((difficulty == 4) ? 500 : 100) }; //Dash towards player if distance is greater than 500 pixels for level 4 or 100 pixels for level 5
		if (offEdge || dashTowardsPlayer) {
			keyIsPressed[dodge] = true;
		}
	}


	//Apply flipchance changes
	for (std::map<int, bool>::iterator it{ keyIsPressed.begin() }; it != keyIsPressed.end(); ++it) {
		float chance{ flipChance };
		if (it->first == dodge) { chance /= 2; }
		else if (it->first == jump) { chance /= 4; }
		else if (it->first == down) { chance /= 16; }
		it->second = ((static_cast<double>(rand()) / RAND_MAX) <= chance) ? !it->second : it->second;
	}


	//Handle input using keyIsPressed
	if (dodgeFramesLeft) { actionable = false; }

	directionKeycodesThisFrame.clear();

	if (keyIsPressed[left]) {
		directionKeycodesThisFrame.push_back(left);
	}
	if (keyIsPressed[right]) {
		directionKeycodesThisFrame.push_back(right);
	}

	if (directionKeycodesThisFrame.empty()) {
		mostRecentDirectionKeycode = -1;
	}
	else if (directionKeycodesLastFrame != directionKeycodesThisFrame) {
		//Most recent key press will be the item in directionKeycodesThisFrame that isn't in directionKeycodesLastFrame (not taking into account multiple keys being pressed down on the same frame)
		for (int kc : directionKeycodesThisFrame) {
			if (std::find(directionKeycodesLastFrame.begin(), directionKeycodesLastFrame.end(), kc) == directionKeycodesLastFrame.end()) {
				//kc was pressed this frame but wasn't pressed last frame, it is the most recent key press
				mostRecentDirectionKeycode = kc;
			}
		}
	}

	directionKeycodesLastFrame = directionKeycodesThisFrame;


	//Pressing either left or right (but not both - case covered by above check)
	if (keyIsPressed[right] || keyIsPressed[left]) {
		//Handle horizontal movement
		if (isGrounded && !dodgeFramesLeft) {
			if (keyIsPressed[right]) {
				velocity.x = topSpeed;
			}
			if (keyIsPressed[left]) {
				velocity.x = -topSpeed;
			}
		}
		//In midair, player can slightly adjust their direction
		else if (!dodgeFramesLeft) {
			if (keyIsPressed[right]) {
				//Jumping to the right
				if (jumpDirection == 1) {
					velocity.x = topSpeed;
				}
				//Travelling to left or straight up, but player is pressing right - let them switch direction to the right but only 30% of the regular speed
				else {
					velocity.x = 0.6 * topSpeed;
				}
			}
			if (keyIsPressed[left]) {
				//Jumping to the left
				if (jumpDirection == -1) {
					velocity.x = -topSpeed;
				}
				//Travelling to right or straight up, but player is pressing right - let them switch direction to the left but only 30% of the regular speed
				else {
					velocity.x = 0.6 * -topSpeed;
				}
			}
		}
	}

	if (!hasKnockback && !dodgeFramesLeft) {
		//Pressing both keys at same time or not pressing either key (and doesn't have knockback)
		if ((keyIsPressed[left] && keyIsPressed[right]) || (!keyIsPressed[left] && (!keyIsPressed[right]))) {
			//Slow down to an immediate hault
			velocity.x = 0;

		}
		//Pressing both keys at the same time, the player should be facing to their most recent key press
		if (keyIsPressed[left] && keyIsPressed[right] && !dodgeFramesLeft) {
			if (flipped && mostRecentDirectionKeycode == right) {
				flipped = false;
				setScale(1, 1);
			}
			else if (!flipped && mostRecentDirectionKeycode == left) {
				flipped = true;
				setScale(-1, 1);
			}
		}
	}

	if (isGrounded && !dodgeFramesLeft) {
		//Jumping
		if (keyIsPressed[jump]) {
			isGrounded = false;
			velocity.y = jumpSpeed;
			velocity.x *= 1.25;
			if (velocity.x > 0) {
				jumpDirection = 1;
			}
			else if (velocity.x < 0) {
				jumpDirection = -1;
			}
			else {
				jumpDirection = 0;
			}
		}

	}

	//Crouching
	if (keyIsPressed[down] && !dodgeFramesLeft) {
		if (!crouched) {
			setSize(sf::Vector2f(getSize().x, getSize().y * 0.5f));
			setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
			setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y / 2));
			crouched = true;
		}
	}
	if (crouched && !dodgeFramesLeft) {
		if (!keyIsPressed[down]) {
			setSize(sf::Vector2f(getSize().x, getSize().y / 0.5f));
			setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
			setPosition(sf::Vector2f(getPosition().x, (getPosition().y - getSize().y / 4) + 1)); //Adding 1 to account for floating-point rounding error that causes player to go up 1 pixel every time
			crouched = false;
		}
	}

	//Dodging
	if (!dodgeCooldownFramesLeft && !dodgeButtonPressed) {
		if (keyIsPressed[dodge] && mostRecentDirectionKeycode != -1) {
			dodgeButtonPressed = true;
			if (mostRecentDirectionKeycode == left) {
				dodgeFramesLeft = totalDodgeFrames;
				setVelocity(-dodgeVelocity, getVelocity().y);
			}
			else if (mostRecentDirectionKeycode == right) {
				dodgeFramesLeft = totalDodgeFrames;
				setVelocity(dodgeVelocity, getVelocity().y);
			}
		}
	}
	if (dodgeButtonPressed && !keyIsPressed[dodge] && !dodgeFramesLeft) {
		dodgeButtonPressed = false;
	}
}