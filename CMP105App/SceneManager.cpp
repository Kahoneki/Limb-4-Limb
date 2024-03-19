#include "SceneManager.h"

SceneManager::SceneManager(sf::RenderWindow* hwnd, Input* in) {
	window = hwnd;
	input = in;

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
	currentScene->handleInput(deltaTime);
	currentScene->update(deltaTime);
	currentScene->render();
}

void SceneManager::UpdateDeltaTime() {
	deltaTime = clock.restart().asSeconds();
}