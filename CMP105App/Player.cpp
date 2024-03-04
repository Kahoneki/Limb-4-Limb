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
	leg.setAttack(2, 5, 16, 60, 30, 105, 230);
	leg2.setAttack(5, 15, 40, 60, 30, 105, 230);
	upcut.setAttack(6, 21, 43, 60, 70, 125, 30);

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
			if (input->isKeyDown(sf::Keyboard::K)) {
				leg.setAttacking(true);
				velocity.x = 0;
				actionable = false;
			}
			if (input->isKeyDown(sf::Keyboard::L)) {
				leg2.setAttacking(true);
				velocity.x = 0;
				actionable = false;
			}
			if (input->isKeyDown(sf::Keyboard::I)) {
				upcut.setAttacking(true);
				isGrounded = false;
				velocity.y = jumpSpeed;
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
	if (leg.getAttacking()) {

		leg.kick(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
	if (leg2.getAttacking()) {

		leg2.kick2(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
	if (upcut.getAttacking()) {

		upcut.upper(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
}



int Player::getHealth() { return health; }

void Player::setHealth(int val) { health = val; }


Attack Player::getJab() { return jab;  }

Attack Player::getleg() { return leg; }

Attack Player::getleg2() { return leg2; }

Attack Player::getupcut() { return upcut; }

