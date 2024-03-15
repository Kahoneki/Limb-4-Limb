#include "Audio.h"

void Audio::playBG() {
	sf::Music music;
	music.openFromFile("Assets/Audio/GuileTheme.ogg");
	music.setLoop(true);
	music.setVolume(20);

	music.play();
}

void Audio::hitAudio() {
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("sound.wav"));

	sf::Sound sound;
	sound.setBuffer(buffer);
	sound.play();
}