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

	attacks[0] = Attack(4, 7, 18, 6, 60, 25, 50, -59, 5);
	attacks[1] = Attack(2, 5, 12, 8, 60, 30, 30, 96, 8);
	attacks[2] = Attack(5, 15, 40, 60, 60, 30, 30, 96, 18);
	attacks[3] = Attack(6, 21, 43, 80, 60, 70, 50, -104, 20);

	stunFramesLeft = 0;


	//Create render texture for player
	playerRenderTexture = new sf::RenderTexture();
	if (!playerRenderTexture->create(125, 225)) { std::cerr << std::format("Error creating render texture for player {}", characterIndex); }

	//Load base player textue and sprite
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
					setPosition(sf::Vector2f(getPosition().x, getPosition().y * 1.5));
					crouched = true;
				}

			}

			if (crouched) {
				if (!input->isKeyDown(down)) {
						setSize(sf::Vector2f(getSize().x, getSize().y / 0.5));
						setPosition(sf::Vector2f(getPosition().x, getPosition().y / 1.5));
						crouched = false;
				}
			}

			//-----GROUND COMBAT-----//
			if (!crouched) {
				if (input->isKeyDown(jab)) {
					attacks[0].setAttacking(true);
					velocity.x = 0;
					isAttacking = true;
				}
				if (input->isKeyDown(kick)) {
					attacks[1].setAttacking(true);
					velocity.x = 0;
				}
				if (input->isKeyDown(sweep)) {
					attacks[2].setAttacking(true);
					velocity.x = 0;
				}
				if (input->isKeyDown(upper)) {
					attacks[3].setAttacking(true);
					isGrounded = false;
					velocity.y = jumpSpeed * 0.5;
					velocity.x = 0;
				}
			}


			//DEBUGGING TEXTURE CHANGING LOGIC - TEMP
			activeLimbs[0] = !input->isKeyDown(sf::Keyboard::Num1);
			activeLimbs[1] = !input->isKeyDown(sf::Keyboard::Num2);
			activeLimbs[2] = !input->isKeyDown(sf::Keyboard::Num3);
			activeLimbs[3] = !input->isKeyDown(sf::Keyboard::Num4);
			UpdateTextures();
			velocity.y -= acceleration * dt;
		}
		//Player is in air, so bring them towards ground
		else {
			velocity.y -= acceleration * dt;
		}

		isAttacking = false;
	}
}


void Player::update(float dt) {


	//Grounded check

	if (!crouched) {
		if (getPosition().y >= 375) {
			setPosition(getPosition().x, 375);
			isGrounded = true;
			velocity.y = 0;
		}
	}
	else {
		if (getPosition().y >= 375 + getSize().y) {
			setPosition(getPosition().x, 375 + getSize().y);
			isGrounded = true;
			velocity.y = 0;
		}
	}


	//Update position
	if (stunFramesLeft) { velocity.x = 0; }
	float xPos = getPosition().x + velocity.x * dt;
	float yPos = getPosition().y - ((velocity.y * dt) + (0.5 * (acceleration * dt * dt))); //s=ut+1/2(at^2)
	setPosition(xPos, yPos);


	//Handle combat
	actionable = !stunFramesLeft;

	for (Attack& atk : attacks) {
		if (atk.getAttacking()) {
			atk.strike(dt, getPosition().x, getPosition().y, flipped);
			actionable = false;
		}
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