#pragma once
#include <SFML/Graphics.hpp>
#include "Framework/GameObject.h"

class Attack
{

public:


	//Constructor/Destructor
	Attack();
	Attack(float start, float act, float rec, int hs, sf::Vector2f kb, sf::Vector2f selfKb, float w, float h, float relx, float rely, int dam);
	~Attack();


	//Getters

	GameObject getHitbox();
	int getCounter();
	float getStartup();
	float getActive();
	float getRecovery();
	int getHitstun();
	bool getAttacking();
	int getDamage();
	sf::Vector2f getKnockback();
	sf::Vector2f getSelfKnockback();
	bool getSelfKnockbackApplied();

	//Setters
	void setHitbox(int width, int height, int xpos, int ypos);
	void setAttacking(bool fighting);
	void setDamage(int power);
	void setHitstun(int stun);
	void setSelfKnockbackApplied(bool val);

	//Handles the actual attack
	void strike(float dt, float player_x, float player_y, bool flip, bool crouch);

private:

	GameObject hitbox;

	// How many frames before the hitbox appears.
	float startup;

	// How many frames the hitbox is out for plus the number of startup frames.
	float active;

	// The total amount of frames for the attack.
	float recovery;

	// Counts frames when an attack happens (taking into account delta time). Used to check which state ( startup, active, recovery ) the attack is in.
	int counter;

	//Counts inbetween frames - used to update counter
	float intermediateFloatCounter;

	//Number of frames opponent is stunned for
	int hitstun;

	// Checks if the player is mid attack
	bool attacking;
#
	//Damage of the attack
	int damage;

	//How far the defending player will be knocked back (positive = away from direction of attack)
	sf::Vector2f knockback;

	//How far the attacking player will be knocked back (positive = away from direction of attack)
	sf::Vector2f selfKnockback;

	//Bool lock - true if self knockback has already been applied
	bool selfKnockbackApplied;


	// Data for the hitbox
	float width;

	float height;

	float relative_xpos;

	float relative_ypos;


};

