// Input class
// Class stores the current state on keyboard and mouse input.
// Store which keys are pressed or released, mouse position and mouse button presses.
// @author Paul Robertson EDIT Erin Michno Hughes

#pragma once
#include <vector>

#include <SFML/Graphics.hpp> //For sf::Uint32

class Input
{
public:
	enum class MouseState { UP, DOWN, PRESSED };
private:
	// Mouse structure, store mouse relevant information
	

	struct Mouse
	{
		int x, y;
		MouseState left, right;
	};
	

public:
	Input();
	// Functions for setting key as pressed, released and checking current state.
	void setKeyDown(int key);
	void setKeyUp(int key);
	bool isKeyDown(int key);
	bool isPressed(int key);
	void update();

	// Functions are mouse input, including getting and setting current position and mouse button presses.
	void setMouseX(int lx);
	void setMouseY(int ly);
	void setMousePosition(int lx, int ly);
	int getMouseX();
	int getMouseY();
	void setLeftMouse(MouseState state);
	bool isLeftMouseDown();
	bool isLeftMousePressed();
	void setRightMouse(MouseState state);
	bool isRightMouseDown();
	bool isRightMousePressed();

	//Functions are for handling text entered
	bool getTextEntered();
	void setTextEntered(bool val);
	sf::Uint32 getText();
	void setText(sf::Uint32 val);
	
private:
	// Array of booleans representing keys (pressed = true, released = false)
	bool keys[256]{ false };
	std::vector<int> pressed;
	// Mouse variable
	Mouse mouse;

	sf::Uint32 text;
	bool textEntered;
};
