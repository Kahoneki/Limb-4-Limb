#include "Level.h"


Level::Level(sf::RenderWindow* hwnd, Input* in) {
	window = hwnd;
	input = in;

	// initialise game objects
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



	InitialiseHealthBars();
	InitialisePlayer1();
	InitialisePlayer2();
}

Level::~Level()
{

}

// handle user input
void Level::handleInput(float dt)
{
	// Close window on Escape pressed.
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
	window->draw(robot);
	for (int i{}; i < 4; ++i) {
		window->draw(robot.getAttack(i).getHitbox());
	}


	window->draw(dummy);
	for (int i{}; i < 4; ++i) {
		window->draw(dummy.getAttack(i).getHitbox());
	}
	window->draw(HealthBarBack1);
	window->draw(HealthBarBack2);
	window->draw(HealthBarFront1);
	window->draw(HealthBarFront2);
	endDraw();

	
	robot.handleInput(dt, sf::Keyboard::Space, sf::Keyboard::A, sf::Keyboard::D, sf::Keyboard::S,  sf::Keyboard::R, sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::T);
	dummy.handleInput(dt, sf::Keyboard::Up, sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Down, sf::Keyboard::O, sf::Keyboard::L, sf::Keyboard::SemiColon, sf::Keyboard::P);


}

// Update game objects
void Level::update(float dt)
{
	robot.update(dt);
	dummy.update(dt);
	dummy.setCollisionBox(dummy.getPosition().x, dummy.getPosition().y, -150, -275);
	for (int i{}; i < 4; ++i) {
		if (dummy.getGlobalBounds().intersects(robot.getAttack(i).getHitbox().getGlobalBounds())) {
			if (!dummy.getStunFramesLeft()){ 
				dummy.setHealth(dummy.getHealth() - robot.getAttack(i).getDamage());
				dummy.setStunFramesLeft(dummy.getAttack(i).getHitstun());
			}
		}

		if (robot.getGlobalBounds().intersects(dummy.getAttack(i).getHitbox().getGlobalBounds())) {
			if (!robot.getStunFramesLeft()) {
				robot.setHealth(robot.getHealth() - dummy.getAttack(i).getDamage());
				robot.setStunFramesLeft(robot.getAttack(i).getHitstun());
			}
		}
	}
	HealthBarUpdate(robot, dummy);

	FlipCheck(robot, dummy);

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


	window->draw(dummy);
	for (int i{}; i < 4; ++i) {
		window->draw(dummy.getAttack(i).getHitbox());
	}
	window->draw(HealthBarBack1);
	window->draw(HealthBarBack2);
	window->draw(HealthBarFront1);
	window->draw(HealthBarFront2);

	endDraw();
}


void Level::HealthBarUpdate(Player play1, Player play2) {
	int Calc1 = 4 * play1.getHealth();
	int Calc2 = 4 * play2.getHealth();

	HealthBarFront1.setSize(sf::Vector2f(Calc1, 50));
	HealthBarFront2.setSize(sf::Vector2f(Calc2, 50));
	HealthBarFront2.setPosition((window->getSize().x - Calc2 - 25), 25);

	if (play1.getHealth() <= 0)
	{
		window->close();
		std::cout << "Player 2 is the winner!" << std::endl;
	}
	if (play2.getHealth() <= 0) 
	{
		window->close();
		std::cout << "Player 1 is the winner!" << std::endl;
	}

}



void Level::FlipCheck(Player& p1, Player& p2) {
	bool playersFacingOppositeDirections { (p1.getScale().x == 1 && p1.getPosition().x > p2.getPosition().x) || (p1.getScale().x == -1 && p1.getPosition().x < p2.getPosition().x) };
	if (playersFacingOppositeDirections) {
		p1.setFlipped(!p1.getFlipped());
		p1.setScale(-p1.getScale().x, 1);
		p2.setFlipped(!p2.getFlipped());
		p2.setScale(-p2.getScale().x, 1);
	}
}


void Level::InitialiseHealthBars() {
	HealthBarFront1.setSize(sf::Vector2f(400, 50));
	HealthBarFront1.setPosition(25, 25);
	HealthBarFront1.setFillColor(sf::Color::Green);

	HealthBarBack1.setSize(sf::Vector2f(400, 50));
	HealthBarBack1.setPosition(25, 25);
	HealthBarBack1.setFillColor(sf::Color::Red);

	HealthBarFront2.setSize(sf::Vector2f(400, 50));
	HealthBarFront2.setPosition((window->getSize().x - 425), 25);
	HealthBarFront2.setFillColor(sf::Color::Green);
	
	HealthBarBack2.setSize(sf::Vector2f(400, 50));
	HealthBarBack2.setPosition((window->getSize().x - 425), 25);
	HealthBarBack2.setFillColor(sf::Color::Red);
}


void Level::InitialisePlayer1() {
	robot = Player(2200.0f, 175.0f, 900.0f, 100, 100, 0, false);
	robot.setSize(sf::Vector2f(150, 275));
	robot.setPosition(175, 375);
	robot.setInput(input);
	robot.setHealth(100);
	robot.setOrigin(robot.getLocalBounds().width / 2.f, robot.getLocalBounds().height / 2.f);
}

void Level::InitialisePlayer2() {
	dummy = Player(2200.0f, 175.0f, 900.0f, 100, 100, 0, true);
	dummy.setSize(sf::Vector2f(150, 275));
	dummy.setPosition(975, 375);
	dummy.setInput(input);
	dummy.setHealth(100);
	dummy.setScale(-1.0f, 1.0f);
	dummy.setOrigin(dummy.getLocalBounds().width / 2.f, dummy.getLocalBounds().height / 2.f);
	dummy.setFillColor(sf::Color::Red);
}
