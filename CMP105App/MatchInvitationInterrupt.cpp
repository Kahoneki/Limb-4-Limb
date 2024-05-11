#include "MatchInvitationInterrupt.h"
#include "NetworkManager.h"
#include "ColourPallete.h"
#include <format>

MatchInvitationInterrupt& MatchInvitationInterrupt::getInstance()
{
	static MatchInvitationInterrupt instance;
	return instance;
}

MatchInvitationInterrupt::MatchInvitationInterrupt()
{
	InitialiseCallbacks();

	if (!font.loadFromFile("font/arial.ttf")) { std::cout << "Error loading font\n"; }
	popupBox = TextBox(500, 400, 920, 280, INACTIVEBOXCOLOUR, TEXTCOLOUR, 40, font, "INCOMING MATCH INVITATION\nUSER: \nRANKING: ");
	acceptMatchInvitation = Button(500, 800, 300, 100, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 40, font, onAcceptMatchInvitationButtonClick, "ACCEPT");
	declineMatchInvitation = Button(1200, 800, 300, 100, INACTIVEBOXCOLOUR, ACTIVEBOXCOLOUR, TEXTCOLOUR, 40, font, onDeclineMatchInvitationButtonClick, "DECLINE");

	networkListener = NetworkManager::getInstance(false).GenerateNetworkListener(*this, NetworkManager::ReservedEntityIndexTable::MATCH_INVITATION_INTERRUPT);
	invitationReceived = false;
	valuesSet = false;
	username = "N/A";
	ranking = -1;
	networkManagerIndex = -1;
	playerNum = -1;

}

void MatchInvitationInterrupt::InitialiseCallbacks()
{
	onAcceptMatchInvitationButtonClick = [this]() {
		sf::Packet outgoingPacket;
		sf::Int8 acceptance{ 0 };
		outgoingPacket << acceptance << networkManagerIndex;
		NetworkManager::getInstance(true).SendDataToServer(NetworkManager::ReservedEntityIndexTable::MATCH_INVITATION_INTERRUPT, PacketCode::MatchAcceptanceClientToServer, outgoingPacket);
		
		//Halt until player num is received from server
		while (playerNum == -1) {
			NetworkManager::getInstance(false).CheckForIncomingDataFromServer();
		}
		std::cout << "PLAYER NUM: " << playerNum << '\n';
		readyToLoadScene = true;
		invitationReceived = false;
	};
	onDeclineMatchInvitationButtonClick = [this]() {
		sf::Packet outgoingPacket;
		sf::Int8 acceptance{ 1 };
		outgoingPacket << acceptance << networkManagerIndex;
		NetworkManager::getInstance(true).SendDataToServer(NetworkManager::ReservedEntityIndexTable::MATCH_INVITATION_INTERRUPT, PacketCode::MatchAcceptanceClientToServer, outgoingPacket);
		invitationReceived = false;
	};
}


void MatchInvitationInterrupt::processEvents(sf::Vector2f mousePos)
{

	if (invitationReceived && !valuesSet) {
		popupBox.text.setString(std::format("INCOMING MATCH INVITATION\nUSER: {}\nRANKING: {}", username, ranking));
		valuesSet = true;
	}

	acceptMatchInvitation.processEvents(mousePos);
	declineMatchInvitation.processEvents(mousePos);
}

void MatchInvitationInterrupt::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(popupBox, states);
	target.draw(acceptMatchInvitation, states);
	target.draw(declineMatchInvitation, states);
}


bool MatchInvitationInterrupt::getInvitationReceived() { return invitationReceived; }

int MatchInvitationInterrupt::getNetworkManagerIndex() { return networkManagerIndex; }

int MatchInvitationInterrupt::getPlayerNum() { return playerNum; }

bool MatchInvitationInterrupt::getReadyToLoadScene() { return readyToLoadScene; }


//std::string MatchInvitationInterrupt::getUsername() { return username; }
//
//sf::Int32 MatchInvitationInterrupt::getRanking() { return ranking; }

