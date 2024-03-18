#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"
#include "Player.h"
#include <string.h>
#include <iostream>


class Level : BaseLevel {
public:
	Level(sf::RenderWindow* hwnd, Input* in);
	~Level();

	void handleInput(float dt) override;
	void update(float dt) override;
	void render() override;

	void HealthBarUpdate(Player play1, Player play2);
private:
	// Level objects

	Player robot;
	Player dummy;
	sf::Texture robotTexture;
	sf::Texture bgTexture;
	GameObject background;
	GameObject HealthBarFront1;
	GameObject HealthBarFront2;
	GameObject HealthBarBack1;
	GameObject HealthBarBack2;
	AudioManager audioManager;

	// Variable to make sure the hitbox only hits the opponent once
	bool struck;

	void FlipCheck(Player& p1, Player& p2);

	void InitialiseHealthBars();
	void InitialisePlayer1();
	void InitialisePlayer2();

};