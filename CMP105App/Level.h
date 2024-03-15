#pragma once

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
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
	GameObject HealthBarFront1;
	GameObject HealthBarFront2;
	GameObject HealthBarBack1;
	GameObject HealthBarBack2;
	sf::RectangleShape titlebox;
	sf::RectangleShape opt1box;
	sf::RectangleShape opt2box;
	sf::RectangleShape opt3box;
	sf::RectangleShape background;
	sf::Font font;
	sf::Text title;
	sf::Text opt1txt;
	sf::Text opt2txt;
	sf::Text opt3txt;

	// Variable to make sure the hitbox only hits the opponent once
	bool struck;

};