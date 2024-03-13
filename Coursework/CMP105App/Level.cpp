#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in, GameState* gs, AudioManager* aud)
{
	window = hwnd;
	input = in;
	gameState = gs;
	audio = aud;

	// initialise game objects
	//robot = Player(2200.0f, 175.0f, 900.0f, 100, 100);
	//robot.setSize(sf::Vector2f(150, 275));
	//robot.setPosition(175, 375);
	//robot.setInput(input);
	//if (!robotTexture.loadFromFile("Assets/Sprites/Robot-Full.png")) { std::cerr << "Robot texture failed to load"; }
	//robot.setTexture(&robotTexture);
	background.setSize(sf::Vector2f(1200, 675));
	background.setPosition(0, 0);
	background.setFillColor(sf::Color::Black);
	titlebox.setSize(sf::Vector2f(520, 60));
	titlebox.setPosition(350, 50);
	titlebox.setFillColor(sf::Color::White);
	opt1box.setSize(sf::Vector2f(350, 40));
	opt1box.setPosition(230, 300);
	opt1box.setFillColor(sf::Color::White);
	opt2box.setSize(sf::Vector2f(320, 40));
	opt2box.setPosition(680, 300);
	opt2box.setFillColor(sf::Color::White);
	opt3box.setSize(sf::Vector2f(235, 40));
	opt3box.setPosition(530, 500);
	opt3box.setFillColor(sf::Color::White);

	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Error loading font\n";
	}

	title.setFont(font);
	title.setString("LOSING LIMBS GAME");
	title.setCharacterSize(50);
	title.setFillColor(sf::Color::Red);
	title.setPosition(350, 50);
	opt1txt.setFont(font);
	opt1txt.setString("SINGLE PLAYER: Press 1");
	opt1txt.setCharacterSize(30);
	opt1txt.setFillColor(sf::Color::Red);
	opt1txt.setPosition(230, 300);
	opt2txt.setFont(font);
	opt2txt.setString("MULTIPLAYER: Press 2");
	opt2txt.setCharacterSize(30);
	opt2txt.setFillColor(sf::Color::Red);
	opt2txt.setPosition(680, 300);
	opt3txt.setFont(font);
	opt3txt.setString("Controls: Press 3");
	opt3txt.setCharacterSize(30);
	opt3txt.setFillColor(sf::Color::Red);
	opt3txt.setPosition(530, 500);

}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	if (input->isKeyDown(sf::Keyboard::Escape))
	{
		window->close();
	}
	int playerchoice = 0;
	bool menubool = true;
	while (menubool = true)
	{
		if (input->isKeyDown(sf::Keyboard::Num1))
		{
			playerchoice = 1;
			menubool = false;
		}
		if (input->isKeyDown(sf::Keyboard::Num2))
		{
			playerchoice = 2;
			menubool = false;
		}
		if (input->isKeyDown(sf::Keyboard::Num3))
		{
			playerchoice = 3;
			menubool = false;
		}
	}

	beginDraw();
	window->clear();
	//window->draw(robot);
	endDraw();
	// Close window on Escape pressed.



	//robot.handleInput(dt);

	
}

// Update game objects
void Level::update(float dt)
{
	//robot.update(dt);
}

// Render level
void Level::render()
{
	beginDraw();


	window->draw(background);
	window->draw(titlebox);
	window->draw(opt1box);
	window->draw(opt2box);
	window->draw(opt3box);


	window->draw(opt1txt);
	window->draw(opt2txt);
	window->draw(opt3txt);
	window->draw(title);
	endDraw();
}

