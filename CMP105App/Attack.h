#pragma once
#include <SFML/Graphics.hpp>
#include "Framework/GameObject.h"

class Attack
{

public:


	//Constructor/Destructor
	Attack();
	~Attack();


	//Getters

	GameObject getHitbox();
	float getCounter();
	float getStartup();
	float getActive();
	float getRecovery();
	bool getAttacking();

	//Setters

	void setAttack(float start, float act, float rec, float w, float h, float relx, float rely);
	void setHitbox(int width, int height, int xpos, int ypos);
	void setCounter(float count);
	void setAttacking(bool fighting);

	//Handles the actual attack
	void strike(float dt, float player_x, float player_y);

private:


	GameObject hitbox;

	// How many frames before the hitbox appears.
	float startup;

	// How many frames the hitbox is out for plus the number of startup frames.
	float active;

	// The total amount of frames for the attack.
	float recovery;

	// Counts frames when an attack happens. Used to check which state ( startup, active, recovery ) the attack is in.
	float counter;

	// Checks if the player is mid attack
	bool attacking;

	// Data for the hitbox
	float width;

	float height;

	float relative_xpos;

	float relative_ypos;
};

