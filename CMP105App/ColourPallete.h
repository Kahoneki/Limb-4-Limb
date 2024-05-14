#ifndef COLOURPALLETE_H
#define COLOURPALLETE_H

#include "SFML/Graphics/Color.hpp"

//Global colours - internal linkage, static duration, constant

static const sf::Color BACKGROUNDCOLOUR{ sf::Color(0x5F0F40FF) };
static const sf::Color INACTIVEBOXCOLOUR{ sf::Color(0xFB8B24FF) };
static const sf::Color ACTIVEBOXCOLOUR{ sf::Color(0xE36414FF) };
static const sf::Color TEXTCOLOUR{ sf::Color(0x9A031EFF) };

static const sf::Color LEVEL1AICOLOUR{ sf::Color::Green };
static const sf::Color LEVEL2AICOLOUR{ sf::Color::Yellow };
static const sf::Color LEVEL3AICOLOUR{ sf::Color::Blue };
static const sf::Color LEVEL4AICOLOUR{ sf::Color::Red };
static const sf::Color LEVEL5AICOLOUR{ sf::Color::Black };

#endif