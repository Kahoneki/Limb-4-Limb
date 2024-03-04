#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "Framework/GameObject.h"
#include "Attack.h"

class Player : public GameObject {
public:

	//Constructor/Destructor
	Player();
	Player(float acc, float ts, float js, int hp, int prot);
	~Player();

	
	//Pipeline
	void handleInput(float dt, int up, int left, int right);
	void update(float dt) override;


	//Getters
	int getHealth();
	int getProtection();
	Attack getJab();
	Attack getleg();
	Attack getleg2();
	Attack getupcut();

	//Setters
	void setHealth(int val);


private:
	//----MEMBERS----//
	//Movement
	float acceleration; //This is vertical acceleration (upwards is positive) - there is no horizontal acceleration
	float topSpeed;
	float jumpSpeed;

	bool isGrounded;
	bool actionable;


	//Combat
	int health;
	int protection;

	Attack jab;
	Attack leg;
	Attack leg2;
	Attack upcut;

	
	
};

#endif