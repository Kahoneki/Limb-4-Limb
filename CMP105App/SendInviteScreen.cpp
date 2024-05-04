#include "SendInviteScreen.h"
#include "MainMenu.h"
#include "SceneManager.h"
#include "NetworkManager.h"
#include "ColourPallete.h"
#include "AccountManager.h"
#include <fstream>

SendInviteScreen::SendInviteScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm), networkManager(NetworkManager::getInstance(false))
{
	std::cout << "Loading send invite screen\n";

	window = hwnd;
	input = in;

	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	int maxUsernameLength = 20;
	usernameBox = InputBox(230, 300, 350, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, maxUsernameLength, InputBox::AcceptableCharacterPresets[InputBox::AcceptableCharacterPreset::ALPHANUMERIC], "Username", false, ACTIVEBOXCOLOUR);
	usernameBox.setInput(input);
	usernameBox.setWindow(window);

	InitialiseCallbacks();
	sendInviteButton = Button(1500, 800, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onSendInviteButtonClick, "SEND INVITE");
	backButton = Button(100, 100, 30, 30, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onBackButtonClick, "<-");
	statusBar = TextBox(230, 800, 700, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");
	displayStatusBar = false;


	awaitServerResponses = false;
	networkListenerIndex = NetworkManager::ReservedEntityIndexTable::SEND_INVITE_SCREEN;
	userExists = -1;
	userOnline = -1;
	userFree = -1;
	userAccept = -1;
	invitedUserNetworkManagerIndex = -1;

	std::cout << "Loaded send invite screen\n";
}

SendInviteScreen::~SendInviteScreen()
{
	std::cout << "Unloading send invite screen\n";
	std::cout << "Unloaded send invite screen\n";
}

void SendInviteScreen::InitialiseCallbacks() {
	onSendInviteButtonClick = [this]() {
		if (!awaitServerResponses) {
			displayStatusBar = true;
			awaitServerResponses = true;

			if (!networkManager.getConnectedToServer()) {
				//Attempt to connect to server
				statusBar.text.setString("Connecting to server...\n");
				render();
				if (!networkManager.AttemptToConnectToServer()) {
					//Connection failed
					statusBar.text.setString("Failed to connect to server");
					awaitServerResponses = false;
				}
			}

			if (networkManager.getConnectedToServer()) {
				networkListener = networkManager.GenerateNetworkListener<SendInviteScreen>(*this, networkListenerIndex);
				sf::Packet outgoingPacket;
				std::string username{ usernameBox.getTypedText() };
				outgoingPacket << username;
				networkManager.SendDataToServer(networkListenerIndex, PacketCode::UsernameInvite, outgoingPacket);
			}

		}
	};


	onBackButtonClick = [this]() {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	};
}


void SendInviteScreen::handleInput(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	usernameBox.processEvents(dt, mousePos);
	sendInviteButton.processEvents(mousePos);
	backButton.processEvents(mousePos);
}


void SendInviteScreen::update(float dt)
{
	if (awaitServerResponses) {
		if (userExists == -1) {
			//Server hasn't responded yet
			networkManager.CheckForIncomingDataFromServer();
		}
		else if (userExists == 1) {
			//User doesn't exist
			statusBar.text.setString("User does not exist.");
			awaitServerResponses = false;
			userExists = -1;
			return;
		}

		//User exists

		statusBar.text.setString("Checking if user is online...");
		if (userOnline == -1) {
			//Server hasn't responded yet
			networkManager.CheckForIncomingDataFromServer();
		}
		else if (userOnline == 1) {
			//User is offline
			statusBar.text.setString("User is offline.");
			awaitServerResponses = false;
			userOnline = -1;
			return;
		}

		//User is online

		statusBar.text.setString("Checking if user is in a match...");
		if (userFree == -1) {
			//Server hasn't responded yet
			networkManager.CheckForIncomingDataFromServer();
		}
		else if (userFree == 1) {
			//User is in a match
			statusBar.text.setString("User is currently in a match.");
			awaitServerResponses = false;
			userFree = -1;
			return;
		}

		//User is free

		statusBar.text.setString("Sending invite to user...");
		if (userAccept == -1) {
			//Server hasn't responded yet
			networkManager.CheckForIncomingDataFromServer();
		}
		else if (userAccept == 1) {
			//User declined match invitation
			statusBar.text.setString("User declined match invitation.");
			awaitServerResponses = false;
			userAccept = -1;
			return;
		}

		//User has accepted match invitation

	}
}


void SendInviteScreen::render()
{
	beginDraw();
	window->draw(background);
	window->draw(usernameBox);
	window->draw(sendInviteButton);
	window->draw(backButton);
	if (displayStatusBar) {
		window->draw(statusBar);
	}
	endDraw();
}