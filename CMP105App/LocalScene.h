#ifndef LOCALSCENE_H
#define LOCALSCENE_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"
#include "SceneManager.h"
#include <string.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"

#include "SceneManager.h"
#include "OnlinePlayer.h"
#include "Platform.h"

class SceneManager; //Forward declaration

class LocalScene : public BaseLevel
{
public:

	LocalScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager);
	~LocalScene();

	void handleInput(float dt) override;
	void update(float dt) override;
	void HealthBarUpdate();
	void render() override;

private:
	SceneManager& sceneManager;

	Player* players[2];
	sf::Texture robotTexture;
	sf::Texture bgTexture;
	sf::RectangleShape background;
	sf::RectangleShape HealthBarFront1;
	sf::RectangleShape HealthBarFront2;
	sf::RectangleShape HealthBarBack1;
	sf::RectangleShape HealthBarBack2;

	Platform platforms[3];

	AudioManager audioManager;

	void FlipCheck();

	void InitialiseScene();
	void InitialisePlayers();
	void InitialiseHealthBars();
};

#endif