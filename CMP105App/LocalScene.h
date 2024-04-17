#ifndef LOCALSCENE_H
#define LOCALSCENE_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/AudioManager.h"
#include <iostream>

#include "Platform.h"

//Forward declarations
class SceneManager;
class Player;

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

	Platform platforms[4];

	AudioManager audioManager;

	//To stop players from falling through platforms at startup - ensure the scene is fully loaded before updating players
	float timeUntilPlayersShouldStartUpdate; //In seconds
	float playerStartUpdateTimeCountdown;

	void InitialiseScene();
	void InitialisePlayers();
	void InitialiseHealthBars();

	void PlatformCollisionCheck(Player* player);
	void AttackHitboxCheck(Player* defendingPlayer, Player* attackingPlayer);
	void ApplyKnockbackToDefendingPlayer(Player* defendingPlayer, Player* attackingPlayer, int limbIndex);
	void ApplyKnockbackToAttackingPlayer(Player* defendingPlayer, Player* attackingPlayer, int limbIndex);
	
	bool debugMode; //Shows colliders
	void DebugRender();
};

#endif