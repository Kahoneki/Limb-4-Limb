# CMP105 2024 Final Submission 

## Game 

**Limb For Limb: This Time It's Global!** 

This individual project extends the group portion by adding networking for online multiplayer, databases for account registration, AI, and new gameplay features.


### **Controls:** 

Left Click : Select

__Local__:

Escape : Pause

**Player 1:**

A/D : Move left/right

W : Jump

S : Crouch

R/F/T/G : Attack

LShift : Dodge/Dash
<br>

**Player 2:**

K/Semicolon : Move left/right

O : Jump

L : Crouch

LBracket/Quote/RBracket/Tilde : Attack

N : Dodge/Dash

<br>
__Online/Singleplayer__:

Escape : Pause

A/D : Move left/right

Space : Jump

S : Crouch

Semicolon/LBracket/RBracket/Enter : Attack

LShift : Dodge/Dash


## Student Details

**Student Name:** Ava Imray
**Student Number:** 2300318

**Course:** CGT

## Changelist

* Added an account system
* Added LAN (extended to full on WiFi functionality)
* Added WiFi capabilities
* Added database connectivity
* Added a singleplayer mode
* Added 5 difficulty levels of AI opponent
* Added platforms
* Added dodge/dash move
* Fine tuned movement to feel more light and bouncy
* Added random bonus items which hold power-ups and debuffs
* Added knockback, both for the attacker and the defender

## Known Bugs
* Online match invitation process can be broken if inviting user disconnects after sending a match invitation and before invited user accepts/declines the invitation.
* When user sends an match invitation, they are stuck on the match invitation screen until the invited user responds.

---
---

# Proposal from March

## Brief Overview of game 

The main feature of my extension for ‘Limb For Limb’ will be the addition of online multiplayer. This will involve either a client-server or peer-to-peer infrastructure and I will conduct proper thorough research to discover the pros and cons of both methods. 
<br><br>
I hope to develop the combat into a direction which is more fun and friendly to all skill levels in line with something like Super Smash Bros. as opposed to the professional competitive fighting games like Street Fighter that the game is based off. This starts with fleshing out the combat to something I’m happy with and then adding things like bonus items / power ups, special abilities, etc.
<br><br>
My extension will allow for online matchmaking based on a player’s skill level (which will be adjusted based on online wins/losses), players’ skill levels will be stored in a database along with any other information that would be helpful to keep track of long-term. I could also look into having this data (specifically player's account information) encrypted.
<br><br>
In addition, I would like to add a single player campaign mode to the game, possibly with unlockable items/characters scattered throughout. This will give me the opportunity to learn about how local save data is managed in games. A singleplayer mode will require the implementation of artificial intelligence controlling the enemies which is another field I’d be interested in learning about.
<br><br>
I’m looking to almost entirely rewrite the structure of the program to accommodate for these new additions, ensuring strong foundations that follow modular principles and functional abstraction.

## Must Have Features

- (REMOVED : RESTRUCTURING THE CODEBASE - NOT A FEATURE)
- Account system : Players will be able to register and log in to an account to keep their data saved. (MOVED FROM SHOULD HAVE TO MUST HAVE)
- LAN : Players will be able to fight each other across different computers in the same local area network.

## Should Have Features

- WiFi Capabilities : Players will be able to fight each other from separate WiFi networks.
- Database Connectivity : A database will be implemented to host player's account information.
- Fun and Friendly : A new combat style will be implemented to promote a more fun and all-skill-levels-friendly feel, including random bonus items / power-ups.

## Could Have Features

- Singleplayer : The player will be able to play the game by themself in some capacity.
- Artificial Intelligence : For any meaningful singleplayer mode, the game will feature some form of Artificial Intelligence to play as the enemy player.

## Wish to Have Features

- Encryption : The database will encrypt players' login information on the database.
- Campaign : The game will feature a fleshed out singleplayer campaign mode with unlockables.
