#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "MainMenu.h"
#include "TimeManager.h"
#include <vector>

class SceneManager
{

public:
	SceneManager(sf::RenderWindow* hwnd, Input* in);
	~SceneManager();

	void LoadScene(BaseLevel* scene);
	void RunCurrentSceneLoop();

private:

	sf::RenderWindow* window;
	Input* input;
	TimeManager& timeManager;

	BaseLevel* currentScene;
};

#endif