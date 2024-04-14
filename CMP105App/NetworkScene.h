#ifndef TESTSCENE_H
#define TESTSCENE_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/GameObject.h"
#include "Framework/AudioManager.h"

#include "SceneManager.h"
#include "OnlinePlayer.h"

class SceneManager; //Forward declaration

class NetworkScene : public BaseLevel
{
public:

	NetworkScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager, int pn);
	~NetworkScene();

	void handleInput(float dt) override;
	void update(float dt) override;
	void HealthBarUpdate();
	void render() override;

private:
	SceneManager& sceneManager;

	OnlinePlayer* players[2];
	sf::Texture robotTexture;
	sf::Texture bgTexture;
	sf::RectangleShape background;
	sf::RectangleShape HealthBarFront1;
	sf::RectangleShape HealthBarFront2;
	sf::RectangleShape HealthBarBack1;
	sf::RectangleShape HealthBarBack2;
	AudioManager audioManager;

	int playerNum;

	void FlipCheck();

	void InitialiseScene();
	void InitialisePlayers();
	void InitialiseHealthBars();
};

#endif