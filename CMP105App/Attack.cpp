#include "Attack.h"
#include "TimeManager.h"
#include <iostream>


//Constructor/destructors
Attack::Attack() {
}

Attack::Attack(float start, float act, float rec, int hs, sf::Vector2f kb, sf::Vector2f selfKb, float w, float h, float relx, float rely, int dam) {

	startup = start;
	active = act;
	recovery = rec;
	counter = 0;
	intermediateFloatCounter = 0;
	invincibilityFrames = hs;
	knockback = kb;
	selfKnockback = selfKb;
	selfKnockbackApplied = false;
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

	intermediateFloatCounter += TimeManager::PhysicsClockFramerate*dt;
	//Check if new frame
	if (static_cast<int>(intermediateFloatCounter) != counter) {
		counter = static_cast<int>(intermediateFloatCounter);
	}

	// If attack is in active frames, create the hitbox
	if (counter > startup && counter < active) {
		if (flip) {
			setHitbox(width, height, player_x - relative_xpos - width, player_y + relative_ypos);
		}
		else {
			setHitbox(width, height, player_x + relative_xpos, player_y + relative_ypos);
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
		intermediateFloatCounter = 0;
	}
}


// Getters
GameObject Attack::getHitbox() { return hitbox; }

int Attack::getCounter() { return counter; }

float Attack::getStartup() { return startup; }

float Attack::getActive() { return active; }

float Attack::getRecovery() { return recovery; }

int Attack::getInvincibilityFrames(){ return invincibilityFrames; }

bool Attack::getAttacking() { return attacking; }

int Attack::getDamage() { return damage; }

sf::Vector2f Attack::getKnockback() { return knockback; }

sf::Vector2f Attack::getSelfKnockback() { return selfKnockback; }

bool Attack::getSelfKnockbackApplied() { return selfKnockbackApplied; }


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

void Attack::setDamage(int power) { damage = power; }

void Attack::setInvincibilityFrames(int stun) { invincibilityFrames = stun; }

void Attack::setSelfKnockbackApplied(bool val) { selfKnockbackApplied = val; }