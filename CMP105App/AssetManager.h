#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H


enum Font
{
	ARIAL
};


//Asset manager class responsible for providing asset references to classes that need them. This stops the same asset being unnecessarily loaded multiple times
class AssetManager
{
	static AssetManager& getInstance();
};

#endif