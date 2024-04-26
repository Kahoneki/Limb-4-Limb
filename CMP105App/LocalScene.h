#ifndef LOCALSCENE_H
#define LOCALSCENE_H

#include <SFML/Graphics.hpp>
#include "Framework/BaseLevel.h"
#include "Framework/Input.h"
#include "Framework/AudioManager.h"
#include <iostream>

#include "Platform.h"

#include "ItemBox.h"

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

	ItemBox* itemBox;
	//Time between item boxes is random, but...
	float minItemBoxCooldownTime; //This is the min amount of time
	float maxItemBoxCooldownTime; //and this is the max amount of time
	float timeUntilNextItemBox; //Initially some random value between minItemBoxCooldownTime and maxItemBoxCooldownTime, but will count down until 0
	
	float itemBoxAbilityCooldownTime; //Time that player will have the ability for
	float timeUntilAbilityEnds; //Counts down until 0

	AudioManager audioManager;


	void InitialiseScene();
	void InitialisePlayers();
	void InitialiseHealthBars();

	void ItemBoxCollisionCheck(Player* player);
	void PlatformCollisionCheck(Player* player);
	void AttackHitboxCheck(Player* defendingPlayer, Player* attackingPlayer);
	void ApplyKnockbackToDefendingPlayer(Player* defendingPlayer, Player* attackingPlayer, int limbIndex);
	void ApplyKnockbackToAttackingPlayer(Player* defendingPlayer, Player* attackingPlayer, int limbIndex);
	
	bool debugMode; //Shows colliders
	void DebugRender();
};

#endif