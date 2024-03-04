#include "Player.h"
#include <iostream>
#include <format>

Player::Player() {
}

//For testing losing limbs
bool numDown = false;

Player::Player(float acc, float ts, float js, int hp, int prot, int c1) {
	acceleration = acc;
	topSpeed = ts;
	jumpSpeed = js;
	health = hp;
	protection = prot;
	characterIndex = c1;

	isGrounded = false;
	actionable = true;

	for (bool& b : activeLimbs) { b = true; }

	jab.setAttack(4, 7, 14, 60, 25, 125, 75);
	leg.setAttack(2, 5, 16, 60, 30, 105, 230);
	leg2.setAttack(5, 15, 40, 60, 30, 105, 230);
	upcut.setAttack(6, 21, 43, 60, 70, 125, 30);

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

}

Player::~Player() {

}



void Player::handleInput(float dt, int jump, int left, int right) {
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
				if (input->isKeyDown(right))
					velocity.x = topSpeed;
				if (input->isKeyDown(left))
					velocity.x = -topSpeed;
			}
			//Jumping
			if (input->isKeyDown(jump)) {
				isGrounded = false;
				velocity.y = jumpSpeed;
				velocity.x *= 1.25;
			}

			//-----GROUND COMBAT-----//
			if (input->isKeyDown(sf::Keyboard::J)) {
				jab.setAttacking(true);
				velocity.x = 0;
				actionable = false;
			}
			if (input->isKeyDown(sf::Keyboard::K)) {
				leg.setAttacking(true);
				velocity.x = 0;
				actionable = false;
			}
			if (input->isKeyDown(sf::Keyboard::L)) {
				leg2.setAttacking(true);
				velocity.x = 0;
				actionable = false;
			}
			if (input->isKeyDown(sf::Keyboard::I)) {
				upcut.setAttacking(true);
				isGrounded = false;
				velocity.y = jumpSpeed;
				velocity.x = 0;
				actionable = false;
			}

			//DEBUGGING TEXTURE CHANGING LOGIC - TEMP
			activeLimbs[0] = !input->isKeyDown(sf::Keyboard::Num1);
			activeLimbs[1] = !input->isKeyDown(sf::Keyboard::Num2);
			activeLimbs[2] = !input->isKeyDown(sf::Keyboard::Num3);
			activeLimbs[3] = !input->isKeyDown(sf::Keyboard::Num4);
			UpdateTextures();

		}
		//Player is in air, so bring them towards ground
		else {
			velocity.y -= acceleration * dt;
		}
	}
}


void Player::update(float dt) {

	//Update position
	float xPos = getPosition().x + velocity.x * dt;
	float yPos = getPosition().y - ((velocity.y * dt) + 0.5 * (acceleration * dt * dt)); //s=ut+1/2(at^2)
	setPosition(xPos, yPos);

	//Grounded check
	if (getPosition().y >= 375) {
		setPosition(getPosition().x, 375);
		isGrounded = true;
	}

	
	// Checks if an attack is in progress and acts accordingly
	if (jab.getAttacking()) {

		jab.strike(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
	if (leg.getAttacking()) {

		leg.kick(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
	if (leg2.getAttacking()) {

		leg2.kick2(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
	}
	if (upcut.getAttacking()) {

		upcut.upper(dt, getPosition().x, getPosition().y);
	}
	else {
		actionable = true;
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
}



int Player::getHealth() { return health; }

void Player::setHealth(int val) { health = val; }


bool Player::getLimbActivity(int index) { return activeLimbs[index]; }

int Player::getLimbRotation(int index) {
	if (activeLimbs[index])
		return aliveLimbSprites[index]->getRotation();
	else
		return deadLimbSprites[index]->getRotation();
}

void Player::UpdateTextures() { updateTextures = true; }

void Player::setLimbActivity(int index, bool val) { activeLimbs[index] = val; }

void Player::setLimbRotation(int index, int rotation) {
	if (activeLimbs[index])
		aliveLimbSprites[index]->setRotation(rotation);
	else
		deadLimbSprites[index]->setRotation(rotation);
}

void Player::addLimbRotation(int index, int rotation) {
	if (activeLimbs[index])
		aliveLimbSprites[index]->rotate(rotation);
	else
		deadLimbSprites[index]->rotate(rotation);
}


Attack Player::getJab() { return jab;  }

Attack Player::getleg() { return leg; }

Attack Player::getleg2() { return leg2; }

Attack Player::getupcut() { return upcut; }

