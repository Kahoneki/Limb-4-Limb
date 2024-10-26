#ifndef TESTSCENE_H
#define TESTSCENE_H


#include "Framework/BaseLevel.h"
#include "Framework/Input.h"

#include "Framework/AudioManager.h"
#include "Platform.h"
#include "PausePopup.h"
#include "TextBox.h"

//Forward declarations
class SceneManager;
class OnlinePlayer;
class NetworkManager;
class TimeManager;
template<typename ParentType>
class NetworkListener;
class ItemBox;

class NetworkScene : public BaseLevel
{
public:

	NetworkScene(sf::RenderWindow* hwnd, Input* in, SceneManager& sceneManager, int pn, int oppNMI);
	~NetworkScene();

	void handleInput(float dt) override;
	void update(float dt) override;
	void HealthBarUpdate();
	void render() override;

	friend class NetworkListener<NetworkScene>;

private:
	SceneManager& sceneManager;

	PausePopup pausePopup;

	OnlinePlayer* players[2];
	sf::Texture robotTexture;
	sf::Texture bgTexture;
	sf::RectangleShape background;
	sf::RectangleShape HealthBarFront1;
	sf::RectangleShape HealthBarFront2;
	sf::RectangleShape HealthBarBack1;
	sf::RectangleShape HealthBarBack2;
	sf::Font font;
	TextBox p1EffectBox;
	TextBox p2EffectBox;

	Platform platforms[4];

	ItemBox* itemBox; //Created by server

	AudioManager audioManager;
	NetworkManager& networkManager;
	TimeManager& timeManager;

	//To stop players from falling through platforms at startup - ensure the scene is fully loaded before updating players
	float timeUntilPlayersShouldStartUpdate; //In seconds
	float playerStartUpdateTimeCountdown;

	int playerNum; //Player num of local player

	bool matchStart; //Stores whether or not both players' scenes have been loaded - halt until this value is true to make sure one player can't start before the other
	int opponentNetworkManagerIndex;

	//Set by server when match is over
	bool matchEnd;
	int winningPlayerNMI;
	int updatedRanking;
	//----//

	NetworkListener<NetworkScene>* networkListener;

	void InitialiseScene();
	void InitialisePlayers();
	void InitialiseHealthBars();

	void ItemBoxCollisionCheck(OnlinePlayer* player);
	void PlatformCollisionCheck(OnlinePlayer* player);
	void AttackHitboxCheck(OnlinePlayer* defendingPlayer, OnlinePlayer* attackingPlayer);
	void ApplyKnockbackToDefendingPlayer(OnlinePlayer* defendingPlayer, OnlinePlayer* attackingPlayer, int limbIndex);
	void ApplyKnockbackToAttackingPlayer(OnlinePlayer* defendingPlayer, OnlinePlayer* attackingPlayer, int limbIndex);

	bool debugMode; //Shows colliders
	void DebugRender();
};

#endif