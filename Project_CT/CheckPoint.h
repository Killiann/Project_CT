#pragma once
#include "SFML/Graphics.hpp"

enum class PosInTile {
	TL, //top left
	TR, //top right
	RT, //right top
	RB, //right bottom
	BR, //bottom right
	BL, //bottom left
	LB, //left bottom
	LT	//left top
};

class CheckPoint {
	int ID = -1;
    int tileSize = 432; //same as in track
	bool isStart = false;

	PosInTile PIT = PosInTile(0);
	sf::Sprite sprite;
	sf::Texture* texture = nullptr;
	sf::RectangleShape collisionRect;

	sf::Vector2f position;
	sf::Vector2f center;
	sf::CircleShape centerShape;	


public:
	CheckPoint(){}
	CheckPoint(int ID, sf::Vector2f pos, PosInTile pit, sf::Texture* texture);	

	inline sf::FloatRect getGlobalBounds() { return collisionRect.getGlobalBounds(); }
	inline int getID() { return ID; }
	inline sf::Vector2f GetCenter() { return center; }

	void Draw(sf::RenderTarget& window, bool isOverlay);
};
