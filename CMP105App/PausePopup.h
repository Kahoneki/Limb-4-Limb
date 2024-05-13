#ifndef PAUSE_POPUP_H
#define PAUSE_POPUP_H

#include "TextBox.h"
#include "Button.h"
#include <functional>


//PausePopup object can be attached to any scene, its processEvents() function should be called every frame in that scene.
//Pressing escape in the scene will cause pausePopupEnabled to be true which the scene should check for every frame (to know when to draw the PausePopup object).
//pausePopupEnabled can also be used to take control away from user.
//If "RESUME" is pressed, pausePopupEnabled will be set to false.
//If "MAIN MENU" is pressed, the user will be returned to the main menu.
//If "QUIT" is pressed, the game will close.


class PausePopup : public GameObject
{
public:
	PausePopup(Input* in);
	void processEvents(sf::Vector2f mousePos); //To be called every frame

	bool getPausePopupEnabled();
	bool getMainMenuButtonClicked();

private:
	Input* input;
	bool escapePressedLastFrame;

	sf::RectangleShape translucentBlackScreenOverlay; //Cover the entire screen with a translucent black overlay to indicate that the game is paused
	sf::Font font;
	TextBox popupBox;
	Button resume;
	Button exitToMainMenu;
	Button quit;

	//Callbacks
	void InitialiseCallbacks();
	std::function<void(void)> resumeOnClick;
	std::function<void(void)> mainMenuOnClick;
	std::function<void(void)> quitOnClick;

	bool pausePopupEnabled;
	bool mainMenuButtonClicked; //To be handled differently if local or online match

	//Override pure virtual draw function
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


#endif