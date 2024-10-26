#include "Framework/Input.h"
#include <iostream>
#include "SceneManager.h"
#include "TimeManager.h"
#include "NetworkManager.h"
#include <random>
#include <ctime>


void windowProcess(sf::RenderWindow* window, Input* in); //Forward declaration

int main()
{
	//Create the window
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Limb For Limb", sf::Style::Fullscreen);
	sf::View view(sf::FloatRect(0, 0, 1920, 1080));
	window.setView(view);
	window.setVerticalSyncEnabled(true);

	srand(time(NULL));

	//Initialise managers
	Input input;
	SceneManager* sceneManager{ new SceneManager(&window, &input) };
	TimeManager& timeManager{ TimeManager::getInstance(240) };
	NetworkManager& networkManager{ NetworkManager::getInstance(false) };


	while (window.isOpen())
	{

		if (timeManager.UpdateDeltaTimeAndCheckForNewFrame()) {
			windowProcess(&window, &input);
			sceneManager->RunCurrentSceneLoop();
			input.update();
		}

		if (networkManager.getConnectedToServer()) {
			networkManager.CheckForIncomingDataFromServer();
		}
	}
}


void windowProcess(sf::RenderWindow* window, Input* in)
{
	// Handle window events.
	sf::Event event;
	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window->close();
			break;
		case sf::Event::KeyPressed:
			// update input class
			in->setKeyDown(event.key.code);
			break;
		case sf::Event::KeyReleased:
			//update input class
			in->setKeyUp(event.key.code);
			break;
		case sf::Event::MouseMoved:
			//update input class
			in->setMousePosition(event.mouseMove.x, event.mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//update input class
				in->setLeftMouse(Input::MouseState::DOWN);
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				in->setRightMouse(Input::MouseState::DOWN);
			}
			break;
		case sf::Event::MouseButtonReleased:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				//update input class
				in->setLeftMouse(Input::MouseState::UP);
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				in->setRightMouse(Input::MouseState::UP);
			}
			break;
		case sf::Event::TextEntered:
			in->setTextEntered(true);
			in->setText(event.text.unicode);
			break;
		default:
			// don't handle other events
			break;
		}
	}

	if (event.type != sf::Event::TextEntered) {
		in->setTextEntered(false);
	}
}