#ifndef PLAYER_H
#define PLAYER_H

#include "Framework/GameObject.h"
#include "Attack.h"

class Player : public GameObject {
public:

	//Constructor/Destructor
	Player();
	Player(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip);
	~Player();

	
	//Pipeline
	void handleInput(float dt, int up, int left, int right, int down, int jab, int kick, int sweep, int upper);
	void update(float dt) override;


	//Getters
	bool getCrouched();
	int getHealth();
	int getMaxHealth();
	int getProtection();
	bool getLimbActivity(int index);
	int getLimbRotation(int index);
	bool getActionable();
	int getInvincibilityFramesLeft();
	bool getFlipped();
	bool getBlocking();
	sf::FloatRect getEffectiveCollider();
	Attack& getAttack(int index);
	bool getGrounded();
	int getCurrentPlatform();
	bool getOnPlatform();
	bool getFallingThroughPlatform();
	bool getHasKnockback();

	//Setters
	void setCrouched(bool val);
	void setHealth(int val);
	void UpdateTextures(); //Call whenever there's a change to activeLimbs[]
	void setLimbActivity(int index, bool val);
	void setLimbRotation(int index, int rotation); //Degrees
	void addLimbRotation(int index, int rotation); //Degrees
	void setFlipped(bool flip);
	void setInvincibilityFramesLeft(int numFrames);
	void setGrounded(bool val);
	void setCurrentPlatorm(int platform);
	void setOnPlatform(bool val);
	void setFallingThroughPlatform(bool val);
	void setHasKnockback(bool val);
	void setJumpDirection(int val);


protected:
	//----MEMBERS----//
	//Movement
	float acceleration; //This is vertical acceleration (upwards is positive) - there is no horizontal acceleration
	float topSpeed;
	float jumpSpeed;
	int jumpDirection; //-1: left, 0: up, 1: right

	int currentPlatform; //Index of platform that player is currently stood on
	bool isOnPlatform;
	bool isFallingThroughPlatform;

	bool isGrounded;
	bool actionable;
	bool crouched;

	//Combat
	sf::Int16 health; //int16 rather than int, since sizeof(int) has to be guaranteed when sending packets
	int maxHealth;
	int protection;
	Attack attacks[4];
	float invincibilityFramesLeft;
	bool blocking;
	bool hasKnockback; //Is currently being knocked back

	//Base
	sf::FloatRect effectiveCollider; //This is the collider that will be used in all collision calculations - it's a bit smaller than getGlobalBounds() and is also in world-space
	sf::Vector2f colliderShrinkage; //Shrink bounds by removing colliderShrinkage.x pixels from each horizontal edge and colliderShrinkage.y pixels from each vertical edge
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