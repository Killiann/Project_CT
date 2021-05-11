#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include "SFML/Graphics.hpp"

enum class TrackType
{
	STRAIGHT,
	CORNER_SHORT,
	CORNER_LONG,
	DOUBLE_CORNER
};

class TrackPiece
{
	const unsigned int trackDivisions = 8;
	unsigned int incr;
	unsigned int tileSize;
	bool shapeRotating = false;

	std::vector<sf::ConvexShape> innerBounds;
	std::vector<sf::ConvexShape> accessibleTrack;
	sf::Sprite drawable;	
	sf::Sprite trackSprite;		

	void InitCorner(int, int, float, float, float);
	void TransformVertices(sf::ConvexShape&, sf::Vector2f, float);

public:
	TrackPiece(sf::Texture& trackTexture, int tileSize, sf::Vector2f gridPos, TrackType trackType, float rotation);	
	std::vector<sf::ConvexShape> GetCollisionShapes();	

	void Draw(sf::RenderTarget& window);
};

