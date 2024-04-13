#include "Player.h"
#include <iostream>
#include <format>

Player::Player() {
}

//For testing losing limbs
bool numDown = false;

Player::Player(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip) {
	setSize(size);
	acceleration = acc;
	topSpeed = ts;
	jumpSpeed = js;
	maxHealth = hp;
	health = hp;
	protection = prot;
	characterIndex = c1;

	isOnPlatform = false;

	isGrounded = false;
	actionable = true;
	crouched = false;

	effectiveCollider = getGlobalBounds();
	colliderShrinkage = sf::Vector2f(10, 0);
	effectiveCollider.left += colliderShrinkage.x;
	effectiveCollider.top += colliderShrinkage.y;
	effectiveCollider.width -= colliderShrinkage.x * 2;
	effectiveCollider.height -= colliderShrinkage.y * 2;

	flipped = flip;
	blocking = false;

	for (bool& b : activeLimbs) { b = true; }
	
	//Attack-hitbox size and relative x and y positions are expressed as percentages of the player's size
	float onePercentX{ getSize().x/100 };
	float onePercentY{ getSize().y/100 };
	attacks[0] = Attack(2, 5,  12, 8,  sf::Vector2f(200, 400),  sf::Vector2f(-50, 200),   onePercentX*70, onePercentY*20,  onePercentX*30, onePercentY*40,    8);  //Kick
	attacks[1] = Attack(5, 15, 40, 60, sf::Vector2f(400, 750),  sf::Vector2f(-200, 350),  onePercentX*70, onePercentY*20,  onePercentX*30, onePercentY*40,   18);  //Sweep
	attacks[2] = Attack(4, 7,  18, 6,  sf::Vector2f(400, 200),  sf::Vector2f(-100, 10),   onePercentX*70, onePercentY*15,  onePercentX*40, onePercentY*-25,   5);  //Jab
	attacks[3] = Attack(2, 15, 30, 70, sf::Vector2f(200, 1000), sf::Vector2f(-100, 300),  onePercentX*55, onePercentY*45,  onePercentX*40, onePercentY*-60,  20);  //Uppercut

	stunFramesLeft = 0;


	//Create render texture for player
	playerRenderTexture = new sf::RenderTexture();
	if (!playerRenderTexture->create(125, 225)) { std::cerr << std::format("Error creating render texture for player {}", characterIndex); }

	//Load base player texture and sprite
	basePlayerTexture = new sf::Texture();
	if (!basePlayerTexture->loadFromFile(std::format("Assets/Sprites/Characters/{}/body.png", characterIndex))) {
		std::cerr << std::format("Failed to load player {} base texture ", characterIndex);
	}
	basePlayerSprite = new sf::Sprite(*basePlayerTexture);
	basePlayerSprite->setPosition(0, 0);
	playerRenderTexture->draw(*basePlayerSprite);

	//Load limb textures and sprites
	for (int i{}; i < 4; ++i) {

		//Alive limb
		aliveLimbTextures[i] = new sf::Texture();
		if (!aliveLimbTextures[i]->loadFromFile(std::format("Assets/Sprites/Characters/{}/Limbs/Alive{}.png", characterIndex, i))) {
			std::cerr << std::format("Failed to load player {} alive limb texture {}", characterIndex, i);
		}
		aliveLimbSprites[i] = new sf::Sprite(*aliveLimbTextures[i]);
		aliveLimbSprites[i]->setPosition(0, 0);
		playerRenderTexture->draw(*aliveLimbSprites[i]);

		//Dead limb
		deadLimbTextures[i] = new sf::Texture();
		if (!deadLimbTextures[i]->loadFromFile(std::format("Assets/Sprites/Characters/{}/Limbs/Dead{}.png", characterIndex, i))) {
			std::cerr << std::format("Failed to load player {} dead limb texture {}", characterIndex, i);
		}
		deadLimbSprites[i] = new sf::Sprite(*deadLimbTextures[i]);
		deadLimbSprites[i]->setPosition(0, 0);
		//Dont draw dead limbs at startup

	}

	playerRenderTexture->display();
	setTexture(&playerRenderTexture->getTexture());

	updateTextures = false;
	blocking = false;

}

Player::~Player() {
	delete playerRenderTexture;

	delete basePlayerTexture;
	delete basePlayerSprite;

	for (sf::Texture* t : aliveLimbTextures) { delete t; }
	for (sf::Sprite* s : aliveLimbSprites) { delete s; }

	for (sf::Texture* t : deadLimbTextures) { delete t; }
	for (sf::Sprite* s : deadLimbSprites) { delete s; }

}


void Player::handleInput(float dt, int jump, int left, int right, int down, int jab, int kick, int sweep, int upper) {

	//-----COMBAT-----//
	if (actionable) {
		if (!crouched) {
			if (input->isKeyDown(kick)) {
				setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
				attacks[0].setAttacking(true);
			}
			if (input->isKeyDown(sweep)) {
				setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
				attacks[1].setAttacking(true);
			}
			if (input->isKeyDown(jab)) {
				setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
				attacks[2].setAttacking(true);
			}
			if (input->isKeyDown(upper)) {
				setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
				attacks[3].setAttacking(true);
			}
		}
	}

	//----MOVEMENT----//
	if (!hasKnockback) {
		//Pressing both keys at same time or not pressing either key (and doesn't have knockback)
		if ((input->isKeyDown(left) && input->isKeyDown(right)) || (!input->isKeyDown(left) && (!input->isKeyDown(right))) ) {
			//Slow down to an immediate hault
			velocity.x = 0;
		}
	}
	//Pressing either left or right (but not both - case covered by above check)
	if (input->isKeyDown(right) || input->isKeyDown(left)) {
		//Handle horizontal movement
		if (isGrounded) {
			if (input->isKeyDown(right)) {
				velocity.x = topSpeed;
			}
			if (input->isKeyDown(left)) {
				velocity.x = -topSpeed;
			}
		}
		//In midair, player can slightly adjust their direction
		else {
			if (input->isKeyDown(right)) {
				//Jumping to the right
				if (jumpDirection == 1) {
					velocity.x = topSpeed;
				}
				//Travelling to left or straight up, but player is pressing right - let them switch direction to the right but only 30% of the regular speed
				else {
					velocity.x = 0.6 * topSpeed;
				}
			}
			if (input->isKeyDown(left)) {
				//Jumping to the left
				if (jumpDirection == -1) {
					velocity.x = -topSpeed;
				}
				//Travelling to right or straight up, but player is pressing right - let them switch direction to the left but only 30% of the regular speed
				else {
					velocity.x = 0.6 * -topSpeed;
				}
			}
		}
	}
	if (isGrounded) {
		//Jumping
		if (input->isKeyDown(jump)) {
			isGrounded = false;
			velocity.y = jumpSpeed;
			velocity.x *= 1.25;
			if (velocity.x > 0) {
				jumpDirection = 1;
			}
			else if (velocity.x < 0) {
				jumpDirection = -1;
			}
			else {
				jumpDirection = 0;
			}
		}

	}

	//Crouching
	if (input->isKeyDown(down)) {
		if (!crouched) {
			setSize(sf::Vector2f(getSize().x, getSize().y * 0.5f));
			setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
			setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y / 2));
			crouched = true;
		}
	}
	if (crouched) {
		if (!input->isKeyDown(down)) {
			setSize(sf::Vector2f(getSize().x, getSize().y / 0.5f));
			setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
			setPosition(sf::Vector2f(getPosition().x, (getPosition().y - getSize().y / 4) + 1)); //Adding 1 to account for floating-point rounding error that causes player to go up 1 pixel every time
			crouched = false;
		}
	}


	//Player is in air, so bring them towards ground
	if (!isGrounded) {
		velocity.y -= acceleration * dt;
	}

}


void Player::update(float dt) {

	//Update position - operating with sf::Vector2i to avoid any potential floating point inaccuracies
	sf::Vector2i currentPos{ static_cast<sf::Vector2i>(getPosition()) };

	currentPos.x += velocity.x * dt;
	if (!isGrounded) {
		currentPos.y -= ((velocity.y * dt) + (0.5 * (acceleration * dt * dt))); //s=ut+1/2(at^2)
	}
	
	//Vertical - check if player has fallen off map
	int deathPlaneLevel { 1920 };
	if (currentPos.y-getSize().y/2 > deathPlaneLevel) {
		health = 0;
	}

	//Horizontal
	if (currentPos.x < getSize().x/2 || currentPos.x > 1920 - getSize().x/2) { //not using <=/>= since velocity.x is being set to 0 and player would be stuck on edge
		currentPos.x = (currentPos.x <= getSize().x/2 ? getSize().x/2 : 1920-getSize().x/2);
		velocity.x = 0;
	}

	setPosition(currentPos.x, currentPos.y);
	effectiveCollider = getGlobalBounds();
	effectiveCollider.left += colliderShrinkage.x;
	effectiveCollider.top += colliderShrinkage.y;
	effectiveCollider.width -= colliderShrinkage.x * 2;
	effectiveCollider.height -= colliderShrinkage.y * 2;
	

	//Ensure player is always facing in the direction they're moving (unless they're being knocked back)
	if (!hasKnockback) {
		if (velocity.x > 0 && getScale() == sf::Vector2f(-1, 1)) { //Moving right, facing left
			//Make player face right
			setScale(1, 1);
			flipped = false;
		}
		else if (velocity.x < 0 && getScale() == sf::Vector2f(1, 1)) { //Moving left, facing right
			//Make player face left
			setScale(-1, 1);
			flipped = true;
		}
		//If player is standing still, don't change the direction they're facing
	}


	//Handle combat
	actionable = !stunFramesLeft;

	for (int i{}; i < 4; ++i) {
		if (attacks[i].getAttacking()) {
			attacks[i].strike(dt, getPosition().x, getPosition().y, flipped, crouched);
			actionable = false;
		}
	}
	//Check that player is actionable and that they're not already opaque
	if (actionable && getFillColor().a != 255) {
		setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 255)); //Restore to full transparency
	}

	//Check if player is being knocked back but has hit the ground again
	if (hasKnockback && isGrounded) {
		hasKnockback = false;
	}

	//Check if limbs are to be destroyed
	for (int i{}; i < 4; ++i) {
		activeLimbs[i] = health >= 20 * (i + 1);
		UpdateTextures();
	}


	if (updateTextures) {
		updateTextures = false;

		//Clear the render texture
		playerRenderTexture->clear(sf::Color::Transparent);

		//Draw base
		playerRenderTexture->draw(*basePlayerSprite);
		playerRenderTexture->display();

		//Draw limbs
		for (int i{}; i < 4; ++i) {
			if (activeLimbs[i]) {
				//Draw alive limb
				playerRenderTexture->draw(*aliveLimbSprites[i]);
			}
			else {
				//Draw dead limb
				playerRenderTexture->draw(*deadLimbSprites[i]);
			}
		}
		playerRenderTexture->display();
		setTexture(&playerRenderTexture->getTexture());
	}

	if (stunFramesLeft > 0) { stunFramesLeft -= Attack::physicsClockFramerate * dt; }
	else if (stunFramesLeft < 0) { stunFramesLeft = 0; }
}


bool Player::getCrouched() { return crouched; }

int Player::getHealth() { return health; }

int Player::getMaxHealth() { return maxHealth; }

void Player::setCrouched(bool val) {
	crouched = val;
	if (val) {
		setSize(sf::Vector2f(getSize().x, getSize().y * 0.5));
		setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
		setPosition(sf::Vector2f(getPosition().x, getPosition().y + 225 / 4));
	}
	else {
		setSize(sf::Vector2f(getSize().x, getSize().y / 0.5));
		setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
		setPosition(sf::Vector2f(getPosition().x, getPosition().y - 225 / 8));
	}
}

void Player::setHealth(int val) { health = val; }

bool Player::getActionable() { return actionable; }

bool Player::getLimbActivity(int index) { return activeLimbs[index]; }

int Player::getLimbRotation(int index) { return activeLimbs[index] ? aliveLimbSprites[index]->getRotation() : deadLimbSprites[index]->getRotation(); }

Attack Player::getAttack(int index) { return attacks[index]; }

bool Player::getGrounded() { return isGrounded; }

int Player::getCurrentPlatform() { return currentPlatform; }

bool Player::getOnPlatform() { return isOnPlatform; }

bool Player::getFallingThroughPlatform() { return isFallingThroughPlatform; }

bool Player::getHasKnockback() { return hasKnockback; }

int Player::getStunFramesLeft() { return stunFramesLeft; }

bool Player::getFlipped() { return flipped; }

bool Player::getBlocking() { return blocking;  }

sf::FloatRect Player::getEffectiveCollider() { return effectiveCollider; }

void Player::UpdateTextures() { updateTextures = true; }

void Player::setFlipped(bool flip) { flipped = flip; }

void Player::setStunFramesLeft(int numFrames) { stunFramesLeft = numFrames; }

void Player::setGrounded(bool val) { isGrounded = val; }

void Player::setCurrentPlatorm(int val) { currentPlatform = val; }

void Player::setOnPlatform(bool val) { isOnPlatform = val; }

void Player::setFallingThroughPlatform(bool val) { isFallingThroughPlatform = val; }

void Player::setHasKnockback(bool val) { hasKnockback = val; }

void Player::setJumpDirection(int val) { jumpDirection = val; }

void Player::setLimbActivity(int index, bool val) { activeLimbs[index] = val; }

void Player::setLimbRotation(int index, int rotation) { activeLimbs[index] ? aliveLimbSprites[index]->setRotation(rotation) : deadLimbSprites[index]->setRotation(rotation); }

void Player::addLimbRotation(int index, int rotation) { activeLimbs[index] ? aliveLimbSprites[index]->rotate(rotation) : deadLimbSprites[index]->rotate(rotation); }
