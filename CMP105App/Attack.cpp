#include "Attack.h"
#include <iostream>


// Base constructor/destructors

Attack::Attack() {
}


Attack::~Attack() {

}

void Attack::strike(float dt, float player_x, float player_y) {

	counter += (40.f * dt);

	// If attack is in active frames, create the hitbox
	if (counter > startup && counter < active) {
		setHitbox(width, 25, player_x + 125, player_y + 75);
	}

	// if attack is out of active frames, remove the hitbox
	if (counter > active) {
		setHitbox(0, 0, 0, 0);
	}

	// if attack is over, become actionable again
	if (counter > recovery) {
		attacking = false;
		counter = 0;
	}
}
// Getters

GameObject Attack::getHitbox() { return hitbox; }

float Attack::getCounter() { return counter; }

float Attack::getStartup() { return startup; }

float Attack::getActive() { return active; }

float Attack::getRecovery() { return recovery; }

bool Attack::getAttacking() { return attacking; }


// Setters


void Attack::setHitbox(int width, int height, int xpos, int ypos) {

	hitbox.setSize(sf::Vector2f(width, height));
	hitbox.setPosition(sf::Vector2f(xpos, ypos));

	hitbox.setFillColor(sf::Color(0, 0, 0, 0));
	hitbox.setOutlineColor(sf::Color::Red);
	hitbox.setOutlineThickness(4);
}


void Attack::setAttack(float start, float act, float rec, float w, float h, float relx, float rely) {

	startup = start;
	active = act;
	recovery = rec;
	counter = 0;
	attacking = false;
	width = w;
	height = h;
	relative_xpos = relx;
	relative_ypos = rely;

}

void Attack::setCounter(float count) {

	counter = count;
}

void Attack::setAttacking(bool fighting) {
	attacking = fighting;
}