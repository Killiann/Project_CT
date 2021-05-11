#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class ResourceManager
{
	//textures
	sf::Texture carTexture;
	sf::Texture driftTexture;
	sf::Texture trackTexture;
	sf::Texture checkpointTexture;
	sf::Texture ddArrowTexture;

	//fonts
	sf::Font consoleFont;
	sf::Font pixelFont;
	sf::Font robotoLight;
	sf::Font robotoRegular;

	//cursors
	sf::Cursor handCursor;
	sf::Cursor textCursor;
	sf::Cursor arrowCursor;

public:
	ResourceManager();
	
	sf::Texture* GetCarTexture() { return &carTexture; }
	sf::Texture* GetDriftTexture() { return &driftTexture; }
	sf::Texture* GetTrackTexture() { return &trackTexture; }
	sf::Texture* GetCheckpointTexture() { return &checkpointTexture; }
	sf::Texture* GetArrowTexture() { return &ddArrowTexture; }

	sf::Font* GetConsoleFont() { return &consoleFont; }
	sf::Font* GetPixelFont() { return &pixelFont; }
	sf::Font* GetRobotoLight() { return &robotoLight; }
	sf::Font* GetRobotoRegular() { return &robotoRegular; }

	sf::Cursor* GetHandCursor() { return &handCursor; }
	sf::Cursor* GetTextCursor() { return &textCursor; }
	sf::Cursor* GetArrowCursor() { return &arrowCursor; }
};

