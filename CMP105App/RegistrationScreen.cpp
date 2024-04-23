#include "RegistrationScreen.h"
#include "MainMenu.h"
#include "SceneManager.h"
#include "NetworkManager.h"
#include <fstream>

RegistrationScreen::RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading registration screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	
	mousePressedLastFrame = false;
	
	minUsernameLength = 4;
	maxUsernameLength = 20;
	usernameBox = InputBox(230, 300, 350, 40, sf::Color::White, sf::Color::Red, 30, font, maxUsernameLength, InputBox::AcceptableCharacterPresets[InputBox::AcceptableCharacterPreset::ALPHANUMERIC], "Username", false, sf::Color::Green);
	usernameBox.setInput(input);
	usernameBox.setWindow(window);

	registerButton = TextBox(1500, 800, 350, 40, sf::Color::White, sf::Color::Red, 30, font, "Register");
	backButton = TextBox(100, 100, 30, 30, sf::Color::White, sf::Color::Red, 30, font, "<-");
	statusBar = TextBox(230, 800, 700, 40, sf::Color::White, sf::Color::Red, 30, font, "");
	displayStatusBar = false;


	awaitServerResponses = false;
	networkListenerIndex = NetworkManager::ReservedEntityIndexTable::REGISTRATION_SCREEN;
	usernameAvailable = -1;
	uuid = 0;

	std::cout << "Loaded registration screen\n";
}


RegistrationScreen::~RegistrationScreen()
{
	std::cout << "Unloading registration screen\n";
	std::cout << "Unloaded registration screen\n";
}


void RegistrationScreen::handleInput(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	usernameBox.processEvents(dt, mousePos);

	bool mouseOverRegisterButton{ registerButton.box.getGlobalBounds().contains(mousePos) };
	bool mouseOverBackButton{ backButton.box.getGlobalBounds().contains(mousePos) };

	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mousePressedLastFrame;

	if (mouseDown) {
		if (mouseOverRegisterButton && !awaitServerResponses) {
			displayStatusBar = true;
			if (checkClientSideUsernameValidity()) {
				awaitServerResponses = true;
				NetworkManager& networkManager{ NetworkManager::getInstance() };
				while (!networkManager.getConnectedToServer()) {
					statusBar.text.setString("Connecting to server...\n");
					render();
				}
				networkListener = networkManager.GenerateNetworkListener<RegistrationScreen>(*this);
				if (uuid != 0) {
					//Write UUID to file
					std::ofstream uuidFile(usernameBox.getTypedText() + ".uuid");
					// Check if the file is opened successfully
					if (uuidFile.is_open()) {
						// Write the string representation of sf::Uint64 to the file
						uuidFile << std::to_string(uuid) << std::endl;

						// Close the file
						uuidFile.close();

						std::cout << "UUID successfully written to output file" << std::endl;
					}
					else {
						std::cerr << "Failed to open output file for writing" << std::endl;
					}
				}
			}
		}
		else if (mouseOverBackButton) {
			MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
			sceneManager.LoadScene(mainMenu);
		}
	}

	mousePressedLastFrame = sf::Mouse::isButtonPressed(sf::Mouse::Left);
}


void RegistrationScreen::update(float dt) {
	if (awaitServerResponses) {
		if (usernameAvailable == -1) {
			//Server hasn't responded yet
			NetworkManager::getInstance().CheckForIncomingDataFromServer();
			return;
		}
		else if (usernameAvailable == 0) {
			statusBar.text.setString("Username not available.");
			awaitServerResponses = false;
			return;
		}

		//Username available
		statusBar.text.setString("Getting UUID.");

	}
}


void RegistrationScreen::render()
{
	beginDraw();
	window->draw(usernameBox);
	window->draw(registerButton);
	window->draw(backButton);
	if (displayStatusBar) {
		window->draw(statusBar);
	}
	endDraw();
}


bool RegistrationScreen::checkClientSideUsernameValidity() {
	if (usernameBox.getTypedText().length() < minUsernameLength) {
		statusBar.text.setString("Username must be longer than " + std::to_string(minUsernameLength) + " characters");
		return false;
	}
	else if (usernameBox.getTypedText().length() > maxUsernameLength) {
		//This condition is just a fail safe and shouldn't be met - username box max length should be set to maxUsernameLength
		statusBar.text.setString("Username cannot be longer than " + std::to_string(maxUsernameLength) + " characters");
		return false;
	}
	return true;
}


void RegistrationScreen::sendUsernameToServer() {
	NetworkManager& networkManager{ NetworkManager::getInstance() };
	while (!networkManager.getConnectedToServer()) {
		statusBar.text.setString("Connecting to server...\n");
		render();
	}
	networkListener = networkManager.GenerateNetworkListener<RegistrationScreen>(*this);
	statusBar.text.setString("Validating username...\n");
	render();
	sf::Packet outgoingPacket;
	outgoingPacket << usernameBox.getTypedText();
	std::cout << "RegistrationScreen: " << usernameBox.getTypedText() << '\n';
	networkManager.SendDataToServer(networkListenerIndex, PacketCode::Username, outgoingPacket);
	while (usernameAvailable == -1) {
		//Server hasn't responded yet
		networkManager.CheckForIncomingDataFromServer();
	}
	//Server has responded
	if (usernameAvailable == 0) {
		//Not available
		statusBar.text.setString("Username not available.");
		render();
		return;
	}
	else if (usernameAvailable == 1) {
		//Available
		statusBar.text.setString("Getting UUID");
		render();
	}

	while (uuid == 0) {
		networkManager.CheckForIncomingDataFromServer();
		//Server hasn't responded yet
	}
	statusBar.text.setString("Account registered!");
	render();
}


void RegistrationScreen::setUsernameAvailable(bool val) { usernameAvailable = val; }

void RegistrationScreen::setUUID(sf::Uint64 val) { uuid = val; }
