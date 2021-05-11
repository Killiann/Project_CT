#include "CheckPoint.h"

CheckPoint::CheckPoint(int id, sf::Vector2f pos, PosInTile pit, sf::Texture* tex) : ID(id), position(pos), PIT(pit), texture(tex) {
	isStart = ID == 0 ? true : false;

	int interval = tileSize / 8;
	position.x = tileSize * pos.x;
	position.y = tileSize * pos.y;

	//sprite setup
	sprite.setPosition(position);
	sprite.setTexture(*texture);
	sprite.scale((float)tileSize / 216, (float)tileSize / 216);
	sprite.setOrigin((float)tileSize / 4, (float)tileSize / 4);
	
	if (isStart) sprite.setTextureRect(sf::IntRect(texture->getSize().x / 2, 0, texture->getSize().x / 2, texture->getSize().y));
	else sprite.setTextureRect(sf::IntRect(0, 0, texture->getSize().x / 2, texture->getSize().y));
	
	sprite.rotate(90 * (1.f + (float)((int)PIT / 2)));

	switch (PIT) {
		case(PosInTile::TL):sprite.setPosition(sprite.getPosition().x - interval * 3, sprite.getPosition().y); break;
		case(PosInTile::TR):sprite.setPosition(sprite.getPosition().x - interval, sprite.getPosition().y); break;
		case(PosInTile::RT):sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y - interval * 3); break;
		case(PosInTile::RB):sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y - interval); break;
		case(PosInTile::BR):sprite.setPosition(sprite.getPosition().x + interval * 3, sprite.getPosition().y); break;
		case(PosInTile::BL):sprite.setPosition(sprite.getPosition().x + interval, sprite.getPosition().y); break;
		case(PosInTile::LB):sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + interval * 3); break;
		case(PosInTile::LT):sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + interval); break;
	}

	//setup collision bounds
	sf::FloatRect spriteBounds = sprite.getGlobalBounds();
	if (spriteBounds.width < spriteBounds.height) {
		collisionRect.setPosition(spriteBounds.left + spriteBounds.width / 2, spriteBounds.top);
		collisionRect.setSize(sf::Vector2f(1, spriteBounds.height));
	}
	else {
		collisionRect.setPosition(spriteBounds.left, spriteBounds.top + spriteBounds.height / 2);
		collisionRect.setSize(sf::Vector2f(spriteBounds.width, 1));
	}
	collisionRect.setFillColor(sf::Color::Red);

	//set center for use in fitness function
	center = sf::Vector2f(collisionRect.getPosition().x + collisionRect.getSize().x / 2, collisionRect.getPosition().y + collisionRect.getSize().y / 2);
	centerShape.setFillColor(sf::Color::Green);
	centerShape.setRadius(5);
	centerShape.setPosition(center);
	centerShape.setOrigin(5, 5);
}

void CheckPoint::Draw(sf::RenderTarget& window, bool isOverlay) {
	window.draw(sprite);
	if (isOverlay) {
		window.draw(collisionRect);
		window.draw(centerShape);
	}
}
