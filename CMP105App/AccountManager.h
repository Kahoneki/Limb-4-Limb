#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <string>
#include "SFML/Graphics.hpp"

//Static class that holds the info for the currently logged in account
class AccountManager
{
public:
	static AccountManager& getInstance();

	//Setters
	void setValues(std::string user, int rank);

	//Getters
	std::string getUsername();
	int getRanking();

private:
	AccountManager(std::string user=username, int rank=ranking);

	static bool valuesDefined; //True if setValues() has ever been called - if false when getInstance() is called, values will be initialised to username="N/A" and ranking=-1

	static std::string username;
	static sf::Int32 ranking;
};

#endif