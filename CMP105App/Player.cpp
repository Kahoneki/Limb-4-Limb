#include "Player.h"
#include <iostream>
#include <format>

Player::Player() {
}

//For testing losing limbs
bool numDown = false;

Player::Player(float acc, float ts, float js, int hp, int prot, int c1, bool flip) {
	acceleration = acc;
	topSpeed = ts;
	jumpSpeed = js;
	health = hp;
	protection = prot;
	characterIndex = c1;

	isGrounded = false;
	actionable = true;
	crouched = false;
	flipped = flip;
	blocking = false;

	for (bool& b : activeLimbs) { b = true; }

	attacks[0] = Attack(2, 5, 12, 8, 60, 30, 30, 96, 8);
	attacks[1] = Attack(5, 15, 40, 60, 60, 30, 30, 96, 18);
	attacks[2] = Attack(4, 7, 18, 6, 60, 25, 50, -59, 5);
	attacks[3] = Attack(2, 15, 30, 70, 60, 70, 50, -104, 20);

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
	//----MOVEMENT----//
	//Handle movement if player is on ground, else they shouldn't be able to change horizontal velocity or jump

	if (actionable) {
		if (isGrounded) {
			//Pressing both keys at same time or not pressing either key
			if ((input->isKeyDown(left) && input->isKeyDown(right)) || (!input->isKeyDown(left) && (!input->isKeyDown(right)))) {
				//Slow down to an immediate hault
				velocity.x = 0;
			}
			//Pressing either left or right (but not both - case covered by above check)
			else if (input->isKeyDown(right) || input->isKeyDown(left)) {
				//Handle movement
				if (input->isKeyDown(right)) {
					velocity.x = topSpeed - (flipped * (0.3 * topSpeed));
					blocking = flipped;
				}
				if (input->isKeyDown(left)) {
					velocity.x = -topSpeed + (!flipped * (0.3 * topSpeed));
					blocking = !flipped;
				}
			}
			//Jumping
			if (input->isKeyDown(jump)) {
				isGrounded = false;
				velocity.y = jumpSpeed;
				velocity.x *= 1.25;
			}

			//Ducking
			if (input->isKeyDown(down)) {
				if (!crouched) {
					setSize(sf::Vector2f(getSize().x, getSize().y * 0.5));
					setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
					setPosition(sf::Vector2f(getPosition().x, getPosition().y + 225 / 4));
					crouched = true;
				}
			}
			if (crouched) {
				if (!input->isKeyDown(down)) {
					setSize(sf::Vector2f(getSize().x, getSize().y / 0.5));
					setOrigin(getLocalBounds().width / 2.f, getLocalBounds().height / 2.f);
					setPosition(sf::Vector2f(getPosition().x, getPosition().y - 225 / 4));
					crouched = false;
				}
			}

			//-----GROUND COMBAT-----//
			if (!crouched) {
				if (input->isKeyDown(kick)) {
					setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
					attacks[0].setAttacking(true);
					velocity.x = 0;
				}
				if (input->isKeyDown(sweep)) {
					setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
					attacks[1].setAttacking(true);
					velocity.x = 0;
				}
				if (input->isKeyDown(jab)) {
					setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
					attacks[2].setAttacking(true);
					velocity.x = 0;
				}
				if (input->isKeyDown(upper)) {
					setFillColor(sf::Color(getFillColor().r, getFillColor().g, getFillColor().b, 128));
					attacks[3].setAttacking(true);
					velocity.x = 0;
				}
			}

			velocity.y -= acceleration * dt;
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

	if (stunFramesLeft) { velocity.x = 0; }
	currentPos.x += velocity.x * dt;
	if (!isGrounded) {
		currentPos.y -= ((velocity.y * dt) + (0.5 * (acceleration * dt * dt))); //s=ut+1/2(at^2)
	}

	//Boundary checks
	//Lots of magic number usage - albeit well commented. This is mainly just to avoid calling getSize() every frame (though this could also be done through a constexpr)
	
	//Vertical
	if (currentPos.y > (crouched ? 431 : 375)) { //375 is the "floor", 375+getSize().x/4 = 431 when crouched
		currentPos.y = (crouched ? 431 : 375);
		isGrounded = true;
		velocity.y = 0;
	}
	//No need for a "roof" check since player is not capable of reaching such a height

	//Horizontal
	if (currentPos.x < 63 || currentPos.x > 1138) { //getSize().x/2 = 63, 1200-getSize().x/2 = 1138 (not using <=/>= since velocity.x is being set to 0 and player would be stuck on edge)
		currentPos.x = (currentPos.x <= 63 ? 63 : 1138);
		velocity.x = 0;
	}


	setPosition(currentPos.x, currentPos.y);


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



int Player::getHealth() { return health; }

void Player::setCrouched(bool val) { crouched = val; }

void Player::setHealth(int val) { health = val; }

bool Player::getActionable() { return actionable; }

bool Player::getLimbActivity(int index) { return activeLimbs[index]; }

int Player::getLimbRotation(int index) { return activeLimbs[index] ? aliveLimbSprites[index]->getRotation() : deadLimbSprites[index]->getRotation(); }

Attack Player::getAttack(int index) { return attacks[index]; }

int Player::getStunFramesLeft() { return stunFramesLeft; }

bool Player::getFlipped() { return flipped; }

bool Player::getBlocking() { return blocking;  }

void Player::UpdateTextures() { updateTextures = true; }

void Player::setFlipped(bool flip) { flipped = flip; }

void Player::setStunFramesLeft(int numFrames) { stunFramesLeft = numFrames; }

void Player::setLimbActivity(int index, bool val) { activeLimbs[index] = val; }

void Player::setLimbRotation(int index, int rotation) { activeLimbs[index] ? aliveLimbSprites[index]->setRotation(rotation) : deadLimbSprites[index]->setRotation(rotation); }

void Player::addLimbRotation(int index, int rotation) { activeLimbs[index] ? aliveLimbSprites[index]->rotate(rotation) : deadLimbSprites[index]->rotate(rotation); }
