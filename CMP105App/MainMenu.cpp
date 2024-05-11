#include "MainMenu.h"
#include "OnlineSelectScreen.h"
#include "LocalScene.h"
#include "SceneManager.h"
#include "AccountManager.h"
#include "MatchInvitationInterrupt.h"
#include "NetworkManager.h"
#include "RegistrationScreen.h"
#include "LoginScreen.h"
#include "ColourPallete.h"
#include "NetworkScene.h"


MainMenu::MainMenu(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm), accountManager(AccountManager::getInstance()), matchInvitationInterrupt(MatchInvitationInterrupt::getInstance()), networkManager(NetworkManager::getInstance(false))
{
	std::cout << "Loading main menu\n";

	window = hwnd;
	input = in;
	
	background.setSize(sf::Vector2f(1920, 1080));
	background.setPosition(0, 0);
	background.setFillColor(BACKGROUNDCOLOUR);
	
	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }

	title = TextBox(350, 50, 520, 60, INACTIVEBOXCOLOUR, TEXTCOLOUR, 50, font, "LOSING LIMBS GAME");
	
	InitialiseCallbacks();
	local = Button(230, 300, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onLocalButtonClick, "LOCAL");
	online = Button(800, 300, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onOnlineButtonClick, "ONLINE");
	registration = Button(230, 500, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onRegistrationButtonClick, "REGISTER");
	login = Button(800, 500, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onLoginButtonClick, "");
	switchOnlineStatus = Button(1500, 800, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onSwitchOnlineStatusButtonClick, "");

	username = TextBox(50, 950, 500, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");
	ranking = TextBox(50, 1000, 500, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");

	if (accountManager.getUsername() != "N/A") {
		//User is logged in
		username.text.setString(accountManager.getUsername());
		ranking.text.setString(std::to_string(accountManager.getRanking()));

		login.text.setString("LOGOUT");
	}
	else {
		//User is logged out
		login.text.setString("LOGIN");
	}

	if (networkManager.getConnectedToServer()) {
		switchOnlineStatus.text.setString("GO OFFLINE");
	}
	else {
		switchOnlineStatus.text.setString("GO ONLINE");
	}

	std::cout << "Loaded main menu\n";
}

MainMenu::~MainMenu()
{
	std::cout << "Unloading main menu\n";
	std::cout << "Unloaded main menu\n";
}

void MainMenu::InitialiseCallbacks() {
	onLocalButtonClick = [this]() {
		LocalScene* localScene = new LocalScene(window, input, sceneManager);
		sceneManager.LoadScene(localScene);
	};
	
	onOnlineButtonClick = [this]() {
		if (accountManager.getUsername() == "N/A") {
			LoginScreen* loginScreen = new LoginScreen(window, input, sceneManager);
			sceneManager.LoadScene(loginScreen);
		}
		else {
			OnlineSelectScreen* onlineSelectScreen = new OnlineSelectScreen(window, input, sceneManager);
			sceneManager.LoadScene(onlineSelectScreen);
		}
	};

	onRegistrationButtonClick = [this]() {
		RegistrationScreen* registrationScreen = new RegistrationScreen(window, input, sceneManager);
		sceneManager.LoadScene(registrationScreen);
	};

	onLoginButtonClick = [this]() {
		if (accountManager.getUsername() == "N/A") {
			//User isn't logged in
			LoginScreen* loginScreen = new LoginScreen(window, input, sceneManager);
			sceneManager.LoadScene(loginScreen);
		}
		else {
			//User is logged in
			sf::Packet outgoingData;
			networkManager.SendDataToServer(-1, PacketCode::Logout, outgoingData);
			accountManager.setValues("N/A", -1);
			login.text.setString("LOGIN");
		}
	};

	onSwitchOnlineStatusButtonClick = [this]() {
		if (networkManager.getConnectedToServer()) {
			//Go offline
			if (networkManager.AttemptToDisconnectFromServer()) {
				switchOnlineStatus.text.setString("GO ONLINE");
				accountManager.setValues("N/A", -1);
				login.text.setString("LOGIN");
			}
		}
		else {
			//Go online
			if (networkManager.AttemptToConnectToServer()) {
				switchOnlineStatus.text.setString("GO OFFLINE");
			}
		}
	};
}



void MainMenu::handleInput(float dt)
{
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };

	if (matchInvitationInterrupt.getInvitationReceived()) {
		matchInvitationInterrupt.processEvents(mousePos);
	}

	else {
		//Only allow user to interact with other buttons if there isn't currently a match invitation pop up
		local.processEvents(mousePos);
		online.processEvents(mousePos);
		registration.processEvents(mousePos);
		login.processEvents(mousePos);
		switchOnlineStatus.processEvents(mousePos);
	}
}

void MainMenu::update(float dt)
{
	if (matchInvitationInterrupt.getReadyToLoadScene()) {
		//Open network scene to start match
		NetworkScene* networkScene{ new NetworkScene(window, input, sceneManager, matchInvitationInterrupt.getPlayerNum(), matchInvitationInterrupt.getNetworkManagerIndex())};
		sceneManager.LoadScene(networkScene);
	}
}

void MainMenu::render()
{
	beginDraw();
	window->draw(background);
	window->draw(title);
	window->draw(local);
	window->draw(online);
	window->draw(registration);
	window->draw(login);
	window->draw(switchOnlineStatus);

	if (accountManager.getUsername() != "N/A") { //N/A is default value
		//User is signed in
		window->draw(username);
		window->draw(ranking);
	}

	if (matchInvitationInterrupt.getInvitationReceived()) {
		//Match invitation has been received
		window->draw(matchInvitationInterrupt);
	}

	endDraw();
}