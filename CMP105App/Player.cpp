#include "Player.h"
#include "TimeManager.h"
#include <format>
#include <iostream>

Player::Player() {
}

//For testing losing limbs
bool numDown = false;

Player::Player(sf::Vector2f size, float acc, float ts, float js, int hp, int prot, int c1, bool flip) {
	setSize(size);
	acceleration = acc;
	terminalVelocity = -1500.0f;
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

	totalDodgeFrames = 6.0f;
	dodgeFramesLeft = 0;
	dodgeCooldownFrames = 10.0f;
	dodgeCooldownFramesLeft = 0;
	dodgeButtonPressed = false;
	dodgeVelocity = 1000.0f;

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
	attacks[3] = Attack(2, 15, 30, 70, sf::Vector2f(200, 800),  sf::Vector2f(-100, 800),  onePercentX*55, onePercentY*45,  onePercentX*40, onePercentY*-60,  20);  //Uppercut

	invincibilityFramesLeft = 0;


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

	effectCooldownTime = 10;
	timeUntilEffectEnds = 0;
	hasEffect = false;
	defaultJumpSpeed = jumpSpeed;
	defaultTopSpeed = topSpeed;
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


void Player::handleInput(float dt, int jump, int left, int right, int down, int dodge, int jab, int kick, int sweep, int upper) {

	if (dodgeFramesLeft) { actionable = false; }

	//-----COMBAT-----//
	if (actionable) {
		if (!crouched) {
			if (input->isKeyDown(kick)) {
				attacks[0].setAttacking(true);
			}
			if (input->isKeyDown(sweep)) {
				attacks[1].setAttacking(true);
			}
			if (input->isKeyDown(jab)) {
				attacks[2].setAttacking(true);
			}
			if (input->isKeyDown(upper)) {
				attacks[3].setAttacking(true);
			}
		}
	}


	directionKeycodesThisFrame.clear();

	if (input->isKeyDown(left)) {
		directionKeycodesThisFrame.push_back(left);
	}
	if (input->isKeyDown(right)) {
		directionKeycodesThisFrame.push_back(right);
	}

	if (directionKeycodesThisFrame.empty()) {
		mostRecentDirectionKeycode = -1;
	}
	else if (directionKeycodesLastFrame != directionKeycodesThisFrame) {
		//Most recent key press will be the item in directionKeycodesThisFrame that isn't in directionKeycodesLastFrame (not taking into account multiple keys being pressed down on the same frame)
		for (int kc : directionKeycodesThisFrame) {
			if (std::find(directionKeycodesLastFrame.begin(), directionKeycodesLastFrame.end(), kc) == directionKeycodesLastFrame.end()) {
				//kc was pressed this frame but wasn't pressed last frame, it is the most recent key press
				mostRecentDirectionKeycode = kc;
			}
		}
	}

	directionKeycodesLastFrame = directionKeycodesThisFrame;




	//----MOVEMENT----//
	//Pressing either left or right (but not both - case covered by above check)
	if (input->isKeyDown(right) || input->isKeyDown(left)) {
		//Handle horizontal movement
		if (isGrounded && !dodgeFramesLeft) {
			if (input->isKeyDown(right)) {
				velocity.x = topSpeed;
			}
			if (input->isKeyDown(left)) {
				velocity.x = -topSpeed;
			}
		}
		//In midair, player can slightly adjust their direction
		else if (!dodgeFramesLeft) {
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
	
	if (!hasKnockback && !dodgeFramesLeft) {
		//Pressing both keys at same time or not pressing either key (and doesn't have knockback)
		if ((input->isKeyDown(left) && input->isKeyDown(right)) || (!input->isKeyDown(left) && (!input->isKeyDown(right))) ) {
			//Slow down to an immediate hault
			velocity.x = 0;

		}
	//Pressing both keys at the same time, the player should be facing to their most recent key press
	if (input->isKeyDown(left) && input->isKeyDown(right) && !dodgeFramesLeft) {
		if (flipped && mostRecentDirectionKeycode == right) {
			flipped = false;
			setScale(1, 1);
		}
		else if (!flipped && mostRecentDirectionKeycode == left) {
			flipped = true;
			setScale(-1, 1);
		}
	}
	}

	if (isGrounded && !dodgeFramesLeft) {
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
	if (input->isKeyDown(down) && !dodgeFramesLeft) {
		if (!crouched) {
			setSize(sf::Vector2f(getSize().x, getSize().y * 0.5f));
			setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
			setPosition(sf::Vector2f(getPosition().x, getPosition().y + getSize().y / 2));
			crouched = true;
		}
	}
	if (crouched && !dodgeFramesLeft) {
		if (!input->isKeyDown(down)) {
			setSize(sf::Vector2f(getSize().x, getSize().y / 0.5f));
			setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
			setPosition(sf::Vector2f(getPosition().x, (getPosition().y - getSize().y / 4) + 1)); //Adding 1 to account for floating-point rounding error that causes player to go up 1 pixel every time
			crouched = false;
		}
	}

	//Dodging
	if (!dodgeCooldownFramesLeft && !dodgeButtonPressed) {
		if (input->isKeyDown(dodge) && mostRecentDirectionKeycode != -1) {
			dodgeButtonPressed = true;
			if (mostRecentDirectionKeycode == left) {
				dodgeFramesLeft = totalDodgeFrames;
				setVelocity(-dodgeVelocity, getVelocity().y);
			}
			else if (mostRecentDirectionKeycode == right) {
				dodgeFramesLeft = totalDodgeFrames;
				setVelocity(dodgeVelocity, getVelocity().y);
			}
		}
	}
	if (dodgeButtonPressed && !input->isKeyDown(dodge) && !dodgeFramesLeft) {
		dodgeButtonPressed = false;
	}



}


void Player::update(float dt) {

	//Player is in air, so bring them towards ground
	if (!isGrounded && !(velocity.y == terminalVelocity)) {
		velocity.y -= acceleration * dt;
	}
	if (velocity.y < terminalVelocity) {
		velocity.y = terminalVelocity;
	}

	//Update position - operating with sf::Vector2i to avoid any potential floating point inaccuracies
	sf::Vector2i currentPos{ static_cast<sf::Vector2i>(getPosition()) };

	currentPos.x += velocity.x * dt;
	if (!isGrounded) {
		//Player has reached terminal velocity, just move them down a fixed amount
		if (velocity.y <= terminalVelocity) {
			currentPos.y -= velocity.y * dt;
		}
		else {
			currentPos.y -= ((velocity.y * dt) + (0.5 * (acceleration * dt * dt))); //s=ut+1/2(at^2)
		}
	}
	
	//Vertical - check if player has fallen off map
	int deathPlaneLevel { 1920 };
	if (currentPos.y-getSize().y/2 > deathPlaneLevel) {
		health = 0;
	}

	////Horizontal
	//if (currentPos.x < getSize().x/2 || currentPos.x > 1920 - getSize().x/2) { //not using <=/>= since velocity.x is being set to 0 and player would be stuck on edge
	//	currentPos.x = (currentPos.x <= getSize().x/2 ? getSize().x/2 : 1920-getSize().x/2);
	//	velocity.x = 0;
	//}

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

	actionable = true;
	for (int i{}; i < 4; ++i) {
		if (attacks[i].getAttacking()) {
			attacks[i].strike(dt, getPosition().x, getPosition().y, flipped, crouched);
			actionable = false;
		}
	}
	
	if ((invincibilityFramesLeft || dodgeFramesLeft) && getFillColor().a != 128) {
		setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128)); //Make transparent
	}
	else if (!(invincibilityFramesLeft || dodgeFramesLeft) && getFillColor().a != 255) {
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

	if (invincibilityFramesLeft > 0) { invincibilityFramesLeft -= TimeManager::PhysicsClockFramerate * dt; }
	else if (invincibilityFramesLeft < 0) { invincibilityFramesLeft = 0; }
	
	if (dodgeFramesLeft > 0) { dodgeFramesLeft -= TimeManager::PhysicsClockFramerate * dt; }
	else if (dodgeFramesLeft < 0) {
		dodgeFramesLeft = 0;
		velocity.x = 0;
		dodgeCooldownFramesLeft = dodgeCooldownFrames;
	}

	if (dodgeCooldownFramesLeft > 0) { dodgeCooldownFramesLeft -= TimeManager::PhysicsClockFramerate * dt; }
	else if (dodgeCooldownFramesLeft < 0) { dodgeCooldownFramesLeft = 0; }




	if (hasEffect) {
		timeUntilEffectEnds -= dt;
	}
	if (timeUntilEffectEnds <= 0) {
		
		hasEffect = false;

		//Revert player to regular stats
		switch (effect.itemDrop) {
		case ItemDrop::JumpDecrease:
		case ItemDrop::JumpIncrease:
			jumpSpeed = defaultJumpSpeed;
			break;
		
		case ItemDrop::SpeedDecrease:
		case ItemDrop::SpeedIncrease:
			topSpeed = defaultTopSpeed;
			break;
		}
	}
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

Attack& Player::getAttack(int index) { return attacks[index]; }

bool Player::getGrounded() { return isGrounded; }

int Player::getCurrentPlatform() { return currentPlatform; }

bool Player::getOnPlatform() { return isOnPlatform; }

bool Player::getFallingThroughPlatform() { return isFallingThroughPlatform; }

bool Player::getHasKnockback() { return hasKnockback; }

int Player::getJumpSpeed() { return jumpSpeed; }

int Player::getTopSpeed() { return topSpeed; }

bool Player::getHasEffect() { return hasEffect; }

int Player::getInvincibilityFramesLeft() { return invincibilityFramesLeft; }

bool Player::getFlipped() { return flipped; }

bool Player::getBlocking() { return blocking;  }

sf::FloatRect Player::getEffectiveCollider() { return effectiveCollider; }

void Player::UpdateTextures() { updateTextures = true; }

void Player::setFlipped(bool flip) { flipped = flip; }

void Player::setInvincibilityFramesLeft(int numFrames) { invincibilityFramesLeft = numFrames; }

void Player::setGrounded(bool val) { isGrounded = val; }

void Player::setCurrentPlatorm(int val) { currentPlatform = val; }

void Player::setOnPlatform(bool val) { isOnPlatform = val; }

void Player::setFallingThroughPlatform(bool val) { isFallingThroughPlatform = val; }

void Player::setHasKnockback(bool val) { hasKnockback = val; }

void Player::setJumpDirection(int val) { jumpDirection = val; }

void Player::setJumpSpeed(int val) { jumpSpeed = val; }

void Player::setTopSpeed(int val) { topSpeed = val; }

void Player::setEffect(Effect val) { effect = val; }

void Player::setHasEffect(bool val) {
	timeUntilEffectEnds = effectCooldownTime;
	hasEffect = val;
}

void Player::setLimbActivity(int index, bool val) { activeLimbs[index] = val; }

void Player::setLimbRotation(int index, int rotation) { activeLimbs[index] ? aliveLimbSprites[index]->setRotation(rotation) : deadLimbSprites[index]->setRotation(rotation); }

void Player::addLimbRotation(int index, int rotation) { activeLimbs[index] ? aliveLimbSprites[index]->rotate(rotation) : deadLimbSprites[index]->rotate(rotation); }
