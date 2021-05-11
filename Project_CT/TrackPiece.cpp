#include "TrackPiece.h"

TrackPiece::TrackPiece(sf::Texture& trackTexture, int tSize, sf::Vector2f gridPos, TrackType trackType, float rotation): tileSize(tSize){	
	float xPos = tileSize * gridPos.x;
	float yPos = tileSize * gridPos.y;
	incr = (int)(tileSize / 8.f);

	float textureTileWidth = trackTexture.getSize().x / 5.f;
	trackSprite.setTexture(trackTexture);		
	trackSprite.setScale((float)tileSize / textureTileWidth, (float)tileSize / textureTileWidth);
	trackSprite.setPosition(xPos, yPos);
	trackSprite.setOrigin(textureTileWidth / 2.f, textureTileWidth / 2.f);
	trackSprite.setRotation(rotation);	

	//setup track piece collision shapes
	switch(trackType){
	case(TrackType::STRAIGHT): {
		trackSprite.setTextureRect(sf::IntRect(textureTileWidth, 0, textureTileWidth, textureTileWidth));

		sf::ConvexShape inner, outer;
		inner.setPointCount(4);
		outer.setPointCount(4);

		//top shape		
		inner.setPoint(0, sf::Vector2f(0.f, 0.f)); //top left
		inner.setPoint(1, sf::Vector2f((float)tileSize, 0.f)); //top right
		inner.setPoint(2, sf::Vector2f((float)tileSize, (float)incr)); //bottom right
		inner.setPoint(3, sf::Vector2f(0.f, (float)incr)); //bottom left

		//bottom shape
		outer.setPoint(0, sf::Vector2f(0.f, (float)incr)); //top left
		outer.setPoint(1, sf::Vector2f((float)tileSize, (float)incr)); //top right
		outer.setPoint(2, sf::Vector2f((float)tileSize, 5.f * (float)incr)); //bottom left
		outer.setPoint(3, sf::Vector2f(0.f, 5.f * (float)incr)); //bottom right

		outer.setFillColor(sf::Color(200, 200, 200));
 		TransformVertices(outer, sf::Vector2f(xPos, yPos), rotation);

		accessibleTrack.push_back(outer);
		break;
	}
	case(TrackType::CORNER_SHORT):
		trackSprite.setTextureRect(sf::IntRect(textureTileWidth * 3, 0, textureTileWidth, textureTileWidth));
		InitCorner(incr, incr * 5, rotation, xPos, yPos);
		break;	
	case(TrackType::CORNER_LONG): 
		trackSprite.setTextureRect(sf::IntRect(textureTileWidth * 4, 0, textureTileWidth, textureTileWidth));
		InitCorner(incr * 3, incr * 7, rotation, xPos, yPos);
		break;	
	case(TrackType::DOUBLE_CORNER):
		trackSprite.setTextureRect(sf::IntRect(textureTileWidth * 2, 0, textureTileWidth, textureTileWidth));
		InitCorner(incr, incr * 5, rotation, xPos, yPos);
		InitCorner(incr, incr * 5, rotation + 180, xPos, yPos);
		break;
	}
}

void TrackPiece::Draw(sf::RenderTarget& window) {
	window.draw(trackSprite);
}

void TrackPiece::InitCorner(int startPos, int endPos, float rotation, float xPos, float yPos) {
	//resolution of corners
	int innerRadiusDefinition = 10;
	int outerRadiusDefinition = 15;

	sf::ConvexShape inner, outer;
	inner.setPointCount(2 + innerRadiusDefinition);
	outer.setPointCount(2 + outerRadiusDefinition);

	for (int i = 0; i < innerRadiusDefinition; ++i) {
		float theta = (((float)M_PI / 2) / innerRadiusDefinition);
		float angle = (theta * i);
		inner.setPoint(i, sf::Vector2f(startPos * cos(angle), startPos * sin(angle)));
	}
	inner.setPoint(innerRadiusDefinition, sf::Vector2f(0.f, (float)startPos));
	inner.setPoint(innerRadiusDefinition + 1, sf::Vector2f(0.f, 0.f));

	for (int i = 0; i < outerRadiusDefinition; ++i) {
		float theta = (((float)M_PI / 2.f) / (float)outerRadiusDefinition);
		float angle = (theta * i);
		outer.setPoint(i, sf::Vector2f(endPos * cos(angle), endPos * sin(angle)));
	}

	outer.setPoint(outerRadiusDefinition, sf::Vector2f(0.f, (float)endPos));

	outer.setFillColor(sf::Color(200, 200, 200));	
	TransformVertices(outer, sf::Vector2f(xPos, yPos), rotation);

	inner.setFillColor(sf::Color::Red);
	TransformVertices(inner, sf::Vector2f(xPos, yPos), rotation);

	innerBounds.push_back(inner);
	accessibleTrack.push_back(outer);
}

void TrackPiece::TransformVertices(sf::ConvexShape &transformable, sf::Vector2f pos, float rotation) {
	transformable.setPosition(tileSize / 2.f, tileSize / 2.f);
	transformable.setOrigin(tileSize / 2.f, tileSize / 2.f);
	
	sf::Transformable tr = (sf::Transformable)transformable;
	tr.setPosition(pos.x, pos.y);
	tr.setOrigin(tileSize / 2.f, tileSize / 2.f);
	tr.rotate(rotation);

	sf::Transform t = tr.getTransform();
	for (unsigned int i = 0; i < transformable.getPointCount(); ++i)
		transformable.setPoint(i, t.transformPoint(transformable.getPoint(i)));
}

std::vector<sf::ConvexShape> TrackPiece::GetCollisionShapes() {
	std::vector<sf::ConvexShape> allShapes;	
	allShapes.reserve(innerBounds.size() + accessibleTrack.size());
	allShapes.insert(allShapes.end(), innerBounds.begin(), innerBounds.end());
	allShapes.insert(allShapes.end(), accessibleTrack.begin(), accessibleTrack.end());
	return allShapes;
}
