#include "RegistrationScreen.h"
#include "MainMenu.h"
#include "SceneManager.h"
#include "NetworkManager.h"
#include "ColourPallete.h"
#include <fstream>

RegistrationScreen::RegistrationScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading registration screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	
	mousePressedLastFrame = false;
	
	minUsernameLength = 4;
	maxUsernameLength = 20;
	usernameBox = InputBox(230, 300, 350, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, maxUsernameLength, InputBox::AcceptableCharacterPresets[InputBox::AcceptableCharacterPreset::ALPHANUMERIC], "Username", false, ACTIVEBOXCOLOUR);
	usernameBox.setInput(input);
	usernameBox.setWindow(window);

	InitialiseCallbacks();
	registerButton = Button(1500, 800, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onRegisterButtonClick, "Register");
	backButton = Button(100, 100, 30, 30, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onBackButtonClick, "<-");
	statusBar = TextBox(230, 800, 700, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");
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

void RegistrationScreen::InitialiseCallbacks() {
	onRegisterButtonClick = [this]() {
		if (!awaitServerResponses) {
			displayStatusBar = true;
			if (checkClientSideUsernameValidity()) {
				awaitServerResponses = true;
				statusBar.text.setString("Connecting to server...\n");
				render();
				NetworkManager& networkManager{ NetworkManager::getInstance() };
				networkListener = networkManager.GenerateNetworkListener<RegistrationScreen>(*this, networkListenerIndex);

				sf::Packet outgoingPacket;
				std::string username{ usernameBox.getTypedText() };
				outgoingPacket << username;
				networkManager.SendDataToServer(networkListenerIndex, PacketCode::Username, outgoingPacket);
			}
		}
	};


	onBackButtonClick = [this]() {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	};
}


void RegistrationScreen::handleInput(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	usernameBox.processEvents(dt, mousePos);
	registerButton.processEvents(mousePos);
	backButton.processEvents(mousePos);
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
			usernameAvailable = -1;
			return;
		}

		//Username available
		statusBar.text.setString("Getting UUID...");

		if (uuid == 0) {
			//Server hasn't responded yet
			NetworkManager::getInstance().CheckForIncomingDataFromServer();
			return;
		}

		else {
			awaitServerResponses = false;
			usernameAvailable = -1;
			uuid = 0;
			//Write UUID to file
			statusBar.text.setString("Writing UUID...");
			render();
			std::ofstream uuidFile(usernameBox.getTypedText() + ".uuid");
			// Check if the file is opened successfully
			if (uuidFile.is_open()) {
				// Write the string representation of sf::Uint64 to the file
				uuidFile << std::to_string(uuid) << std::endl;

				// Close the file
				uuidFile.close();

				std::cout << "UUID successfully written to output file" << std::endl;
				statusBar.text.setString("Account successfully created!");

			}
			else {
				std::cerr << "Failed to open output file for writing" << std::endl;
			}
		}
	}
}


void RegistrationScreen::render()
{
	beginDraw();
	window->draw(background);
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
		statusBar.text.setString("Username must be at least " + std::to_string(minUsernameLength) + " characters");
		return false;
	}
	else if (usernameBox.getTypedText().length() > maxUsernameLength) {
		//This condition is just a fail safe and shouldn't be met - username box max length should be set to maxUsernameLength
		statusBar.text.setString("Username cannot be longer than " + std::to_string(maxUsernameLength) + " characters");
		return false;
	}
	return true;
}


void RegistrationScreen::setUsernameAvailable(bool val) { usernameAvailable = val; }

void RegistrationScreen::setUUID(sf::Uint64 val) { uuid = val; }
