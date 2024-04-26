#include "LoginScreen.h"
#include "AccountManager.h"
#include "MainMenu.h"
#include "SceneManager.h"
#include "NetworkManager.h"
#include "ColourPallete.h"
#include <fstream>
#include <filesystem> //For std::filesystem::exists

LoginScreen::LoginScreen(sf::RenderWindow* hwnd, Input* in, SceneManager& sm) : sceneManager(sm)
{
	std::cout << "Loading login screen\n";

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
	loginButton = Button(1500, 800, 350, 40, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onLoginButtonClick, "Login");
	backButton = Button(100, 100, 30, 30, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, onBackButtonClick, "<-");
	statusBar = TextBox(230, 800, 700, 40, INACTIVEBOXCOLOUR, TEXTCOLOUR, 30, font, "");
	displayStatusBar = false;


	awaitServerResponses = false;
	networkListenerIndex = NetworkManager::ReservedEntityIndexTable::LOGIN_SCREEN;
	loginStatus = -1;

	std::cout << "Loaded login screen\n";
}

LoginScreen::~LoginScreen()
{
	std::cout << "Unloading registration screen\n";
	std::cout << "Unloaded registration screen\n";
}

void LoginScreen::InitialiseCallbacks() {
	onLoginButtonClick = [this]() {
		if (!awaitServerResponses) {
			displayStatusBar = true;
			if (checkClientSideUsernameValidity()) {
				awaitServerResponses = true;
				statusBar.text.setString("Connecting to server...\n");
				render();
				NetworkManager& networkManager{ NetworkManager::getInstance() };
				networkListener = networkManager.GenerateNetworkListener<LoginScreen>(*this, networkListenerIndex);

				sf::Packet outgoingPacket;
				outgoingPacket << username << uuid;
				networkManager.SendDataToServer(networkListenerIndex, PacketCode::Login, outgoingPacket);
			}
		}
	};


	onBackButtonClick = [this]() {
		MainMenu* mainMenu = new MainMenu(window, input, sceneManager);
		sceneManager.LoadScene(mainMenu);
	};
}

bool LoginScreen::checkClientSideUsernameValidity() {
	if (usernameBox.getTypedText().length() < minUsernameLength) {
		statusBar.text.setString("Username must be at least " + std::to_string(minUsernameLength) + " characters");
		return false;
	}
	else if (usernameBox.getTypedText().length() > maxUsernameLength) {
		//This condition is just a fail safe and shouldn't be met - username box max length should be set to maxUsernameLength
		statusBar.text.setString("Username cannot be longer than " + std::to_string(maxUsernameLength) + " characters");
		return false;
	}

	username = usernameBox.getTypedText();

	//Check for uuid in format username.uuid
	statusBar.text.setString("Searching for UUID...");
	render();
	std::string filename{ username + ".uuid"};
	if (!std::filesystem::exists(filename)) {
		statusBar.text.setString("No matching UUID file could be found.");
		return false;
	}
	statusBar.text.setString("Reading UUID...");
	std::ifstream uuidFile(filename);
	std::string readData;
	std::getline(uuidFile, readData);
	uuid = std::stoi(readData);

	return true;
}


void LoginScreen::handleInput(float dt) {
	sf::Vector2f mousePos{ window->mapPixelToCoords(sf::Mouse::getPosition(*window)) };
	usernameBox.processEvents(dt, mousePos);
	loginButton.processEvents(mousePos);
	backButton.processEvents(mousePos);
}


void LoginScreen::update(float dt) {
	if (awaitServerResponses) {
		if (loginStatus == -1) {
			//Server hasn't responded yet
			NetworkManager::getInstance().CheckForIncomingDataFromServer();
			return;
		}
		else if (loginStatus == 0) {
			statusBar.text.setString("Username does not match UUID.");
			awaitServerResponses = false;
			loginStatus = -1;
			return;
		}

		//Username matches uuid, log in user

		statusBar.text.setString("Getting ranking...");

		//Get ranking
		if (ranking == -1) {
			//Server hasn't responded yet
			NetworkManager::getInstance().CheckForIncomingDataFromServer();
			return;
		}

		//Username has responded with account's ranking
		awaitServerResponses = false;
		AccountManager::getInstance().setValues(username, ranking);
		std::cout << "Username: " << username << '\n' << "Ranking: " << ranking << '\n';

	}
}


void LoginScreen::render()
{
	beginDraw();
	window->draw(background);
	window->draw(usernameBox);
	window->draw(loginButton);
	window->draw(backButton);
	if (displayStatusBar) {
		window->draw(statusBar);
	}
	endDraw();
}

void LoginScreen::setLoginStatus(bool val) { loginStatus = val; }
void LoginScreen::setRanking(int val) { ranking = val; };