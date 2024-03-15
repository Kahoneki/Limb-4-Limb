#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Audio.h"
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

};