#include "Attack.h"
#include <iostream>


// Base constructor/destructors

Attack::Attack() {
}

Attack::Attack(float start, float act, float rec, int hs, float w, float h, float relx, float rely, int dam) {

	startup = start;
	active = act;
	recovery = rec;
	counter = 0;
	hitstun = hs;
	attacking = false;
	width = w;
	height = h;
	relative_xpos = relx;
	relative_ypos = rely;
	damage = dam;
}


Attack::~Attack() {
}

void Attack::strike(float dt, float player_x, float player_y, bool flip, bool crouch) {

	counter += physicsClockFramerate*dt;

	// If attack is in active frames, create the hitbox
	if (counter > startup && counter < active) {
		if (flip) {
			setHitbox(width, height, player_x - relative_xpos - width, player_y + (crouch ? relative_ypos / 2 : relative_ypos));
		}
		else {
			setHitbox(width, height, player_x + relative_xpos, player_y + (crouch ? relative_ypos / 2 : relative_ypos));
		}
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

int Attack::getHitstun(){ return hitstun; }

bool Attack::getAttacking() { return attacking; }

int Attack::getDamage() { return damage; }


// Setters


void Attack::setHitbox(int width, int height, int xpos, int ypos) {

	hitbox.setSize(sf::Vector2f(width, height));
	hitbox.setPosition(sf::Vector2f(xpos, ypos));

	hitbox.setFillColor(sf::Color(0, 0, 0, 0));
	hitbox.setOutlineColor(sf::Color::Red);
	hitbox.setOutlineThickness(4);
}


void Attack::setAttacking(bool fighting) {
	attacking = fighting;
}