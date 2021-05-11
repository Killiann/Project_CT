#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "TrackPiece.h"
#include "ResourceManager.h"
#include "CheckPoint.h"
#include "CheckPointManager.h"

class Track
{
	enum TrackGuide {
		Su,  //straight up			- 1
		Sr,  //straight right		- 2 
		Sd,  //straight down		- 3
		Sl,  //straight left		- 4 
		CSu, //corner short up		- 5 
		CSr, //corner short right	- 6 
		CSd, //corner short down	- 7 
		CSl, //corner short left	- 8 
		CLu, //corner long up		- 9 
		CLr, //corner long right	- 10 
		CLd, //corner long down		- 11
		CLl, //corner long left		- 12 
		DCu, //double corner up		- 13 
		DCr, //double corner right	- 14		
	};

	static const int tileSize = 432; //texture rect size * 2
	static const int pieceDivisions = 8;
	static const int trackWidth = 4;

	std::vector<TrackPiece> trackPieces;
	std::vector<sf::ConvexShape> trackShapes;

	unsigned int trackGridH, trackGridW;
	std::vector<std::vector<int>> gridVals;

	CheckPointManager cpManager;
public:
	Track(ResourceManager *resource, std::vector<CheckPoint> checkPoints, std::vector<std::vector<int>> grid, unsigned int trackW, unsigned int trackH);

	inline std::vector<sf::ConvexShape>* GetTrackShapes() { return &trackShapes; }
	inline CheckPointManager* GetCheckPoints() { return &cpManager; }
	inline int GetTileWidth() { return tileSize; }

	void Draw(sf::RenderTarget& window, bool devOverview);
};

