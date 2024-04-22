#ifndef INPUTBOX_H
#define INPUTBOX_H

#include "TextBox.h"

#include <string>


class InputBox : public TextBox
{
public:
	InputBox(); //Empty input box
	InputBox(sf::Vector2f pos, sf::Vector2f size, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, int maxTextLength, const char* acceptableChars, const char* defaultTxt="", bool asterisks=false, sf::Color activeCol=sf::Color::Transparent);
	InputBox(float posX, float posY, float sizeX, float sizeY, sf::Color fillCol, sf::Color txCol, float txSize, sf::Font& txFont, int maxTextLength, const char* acceptableChars, const char* defaultTxt="", bool asterisks=false, sf::Color activeCol = sf::Color::Transparent);

	void processEvents(float dt, sf::Vector2f mousePos); //To be called every frame
	void resize();

	const char* getTypedText();

	//Used to index into AcceptableCharacterPresets
	static enum AcceptableCharacterPreset
	{
		ALPHANUMERIC,
		EXTENDED_ALPHANUMERIC,
		ALPHANUMERIC_WITH_SPACE,
		EXTENDED_ALPHANUMERIC_WITH_SPACE
	};

	//Someone using this class can either provide their own acceptable character sets or use one of these presets
	static constexpr const char* AcceptableCharacterPresets[] =
	{
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ",
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_- "
	};

private:
	int maxLength; //How many characters the input box should be able to support
	const char* acceptableCharacters; //These are the "list" of characters that are allowed to be entered into this input box
	std::string defaultText;
	std::string typedText;
	std::string displayText; //Same as typedText but with a | at the end to represent a cursor and with characters replaced by asterisks if displayAsAsterisk is true
	bool displayAsAsterisks;
	sf::Color activeColour; //If active colour param provided, use it - otherwise, just use normal fill colour
	sf::Color inactiveColour;
	float defaultWidth;
	bool active; //User has clicked on input box

	float backspaceCooldown; //In seconds - e.g. if 0.2, backspace can only be pressed once every 0.2 seconds, stopping it from being repeatedly pressed every frame, still letting it be held
	float backspaceCooldownTimer; //When timer reaches 0, backspace can be pressed again

	bool mousePressedLastFrame; //Avoid check for mouse clicking on input box from passing every frame
};

#endif