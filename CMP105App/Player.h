#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "Framework/GameObject.h"
#include "Attack.h"

class Player : public GameObject {
public:

	//Constructor/Destructor
	Player();
	Player(float acc, float ts, float js, int hp, int prot, int c1, bool flip);
	~Player();

	
	//Pipeline
	void handleInput(float dt, int up, int left, int right, int down, int jab, int kick, int sweep, int upper);
	void update(float dt) override;


	//Getters
	int getHealth();
	int getProtection();
	bool getLimbActivity(int index);
	int getLimbRotation(int index);
	bool getActionable();
	int getStunFramesLeft();
	bool getFlipped();
	bool getBlocking();
	Attack getAttack(int index);

	//Setters
	void setHealth(int val);
	void UpdateTextures(); //Call whenever there's a change to activeLimbs[]
	void setLimbActivity(int index, bool val);
	void setLimbRotation(int index, int rotation); //Degrees
	void addLimbRotation(int index, int rotation); //Degrees
	void setFlipped(bool flip);
	void setStunFramesLeft(int numFrames);


private:
	//----MEMBERS----//
	//Movement
	float acceleration; //This is vertical acceleration (upwards is positive) - there is no horizontal acceleration
	float topSpeed;
	float jumpSpeed;

	bool isGrounded;
	bool actionable;
	bool crouched;

	//Combat
	int health;
	int protection;
	bool isAttacking;
	Attack attacks[4];
	float stunFramesLeft;
	bool blocking;

	//Base
	int characterIndex;
	bool flipped;

	//Sprite
	//in order: left arm, right arm, left leg, right leg
	bool activeLimbs[4];

	sf::Texture* basePlayerTexture;
	sf::Sprite* basePlayerSprite;

	sf::Texture* aliveLimbTextures[4];
	sf::Sprite* aliveLimbSprites[4];

	sf::Texture* deadLimbTextures[4];
	sf::Sprite* deadLimbSprites[4];

	sf::RenderTexture* playerRenderTexture;

	bool updateTextures;
	
};

#endif