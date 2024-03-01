#include "Player.h"
#include <iostream>

Player::Player() {
}

Player::Player(float acc, float ts, float js, int hp, int prot) {
	acceleration = acc;
	topSpeed = ts;
	jumpSpeed = js;
	health = hp;
	protection = prot;

	isGrounded = false;
	actionable = true;

	jab.setAttack(4, 7, 14, 60, 25, 125, 75);

}

Player::~Player() {

}



void Player::handleInput(float dt, int jump, int left, int right) {
	//----MOVEMENT----//
	//Handle movement if player is on ground, else they shouldn't be able to change horizontal velocity or jump
	if (actionable) {
		if (isGrounded) {
			//Pressing both keys at same time or not pressing either key
			if ((input->isKeyDown(left) && input->isKeyDown(right)) || (!input->isKeyDown(left) && (!input->isKeyDown(right)))) {
				//Slow down to an immediate hault
				velocity.x = 0;
			}
			//Pressing either left or right (but not both - case covered by above check)
			else if (input->isKeyDown(right) || input->isKeyDown(left)) {
				//Handle movement
				if (input->isKeyDown(right))
					velocity.x = topSpeed;
				if (input->isKeyDown(left))
					velocity.x = -topSpeed;
			}
			//Jumping
			if (input->isKeyDown(jump)) {
				isGrounded = false;
				velocity.y = jumpSpeed;
				velocity.x *= 1.25;
			}

			//-----GROUND COMBAT-----//
			if (input->isKeyDown(sf::Keyboard::J)) {
				jab.setAttacking(true);
				velocity.x = 0;
				actionable = false;
			}
		}
		//Player is in air, so bring them towards ground
		else {
			velocity.y -= acceleration * dt;
		}
	}
}


void Player::update(float dt) {

	//Update position
	float xPos = getPosition().x + velocity.x * dt;
	float yPos = getPosition().y - ((velocity.y * dt) + 0.5 * (acceleration * dt * dt)); //s=ut+1/2(at^2)
	setPosition(xPos, yPos);

	//Grounded check
	if (getPosition().y >= 375) {
		setPosition(getPosition().x, 375);
		isGrounded = true;
	}

	
	// Checks if an attack is in progress and acts accordingly
	if (jab.getAttacking()) {

		jab.strike(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
}



int Player::getHealth() { return health; }

void Player::setHealth(int val) { health = val; }


Attack Player::getJab() { return jab;  }

