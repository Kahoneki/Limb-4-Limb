#include "AccountManager.h"

bool AccountManager::valuesDefined = false;
std::string AccountManager::username = "";
sf::Int32 AccountManager::ranking = 0;

AccountManager& AccountManager::getInstance() {
	if (valuesDefined) {
		static AccountManager instance;
		return instance;
	}
	else {
		//Error: use default values
		static AccountManager instance("N/A", -1);
		return instance;
	}
}

AccountManager::AccountManager(std::string user, int rank) {
	username = user;
	ranking = rank;
}


void AccountManager::setValues(std::string user, int rank) {
	valuesDefined = true;
	username = user;
	ranking = rank;
}

std::string AccountManager::getUsername() { return username; }

int AccountManager::getRanking() { return ranking; }
