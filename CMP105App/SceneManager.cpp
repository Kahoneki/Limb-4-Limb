#include "SceneManager.h"

SceneManager::SceneManager(sf::RenderWindow* hwnd, Input* in, TimeManager* tm) {
	window = hwnd;
	input = in;
	timeManager = tm;

	//Start main menu
	MainMenu* mainMenu = new MainMenu(window, input, *this);
	currentScene = mainMenu;
}

SceneManager::~SceneManager() {
	delete currentScene;
}

void SceneManager::LoadScene(BaseLevel* level) {
	delete currentScene;
	currentScene = level;
}

void SceneManager::RunCurrentSceneLoop() {
	currentScene->handleInput(timeManager->getDeltaTime());
	currentScene->update(timeManager->getDeltaTime());
	currentScene->render();
}