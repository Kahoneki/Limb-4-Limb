#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "MainMenu.h"
#include <vector>

class SceneManager
{

public:
	SceneManager(sf::RenderWindow* hwnd, Input* in);
	~SceneManager();

	void LoadScene(BaseLevel* scene);
	void RunCurrentSceneLoop();
	void UpdateDeltaTime();

private:

	sf::RenderWindow* window;
	Input* input;

	BaseLevel* currentScene;
	sf::Clock clock;
	float deltaTime;
};

#endif