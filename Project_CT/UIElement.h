#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include "ResourceManager.h"

class UIElement {

protected:
	ResourceManager* resourceManager = nullptr; 

	//data
	sf::Vector2f position;
	sf::Vector2f size;
	sf::Font* font = nullptr;
	sf::RectangleShape background;
	sf::Text text;

	float outlineThickness = 1.f;

	//colors
	sf::Color color;
	sf::Color textColor;
	sf::Color hoverColor;
	sf::Color clickColor;
	sf::Color accentColor;

	bool isHovering = false;
	bool isClicking = false;
	bool hidden = false;

	bool hoverable = true;
	bool clickable = true;
	bool onScreen = false;

	//cursors
	sf::Cursor* handCursor = nullptr;
	sf::Cursor* textCursor = nullptr;
	sf::Cursor* arrowCursor = nullptr;
	
	unsigned int cursorId = 0;

	//helpers
	inline virtual void Hovering(bool hovering) {
		isHovering = hovering;
		hovering ? background.setFillColor(hoverColor) : background.setFillColor(color);
	}
	inline virtual void Clicking(bool clicking) {
		isClicking = clicking;
		clicking ? background.setFillColor(clickColor) : background.setFillColor(hoverColor);
	}
	virtual void OnClick() {};

public:
	UIElement() {}; //default
	UIElement(sf::Vector2f pos, sf::Vector2f size, ResourceManager* resource);
	virtual void Update(sf::RenderWindow& window, sf::Event& event);
	virtual void Draw(sf::RenderTarget& window);

	//set styling
	inline void SetPosition(sf::Vector2f p) { position = p; }
	inline void SetSize(sf::Vector2f s) { size = s; }
	inline void SetHoverColor(sf::Color c) { hoverColor = c; }
	inline void SetClickColor(sf::Color c) { clickColor = c; }
	inline void SetFontScale(float scale) { text.setScale(scale, scale); }
	inline void SetText(std::string newString) { text.setString(newString); }
	inline void SetOutlineThickness(float t) { 
		outlineThickness = t; 
		background.setOutlineThickness(t);
	}
	inline void SetColor(sf::Color c) {
		color = c;
		background.setFillColor(color);
	}
	inline void SetAccentColor(sf::Color c) { 
		accentColor = c;
		background.setOutlineColor(accentColor);
	}
	inline void SetTextColor(sf::Color c) {
		textColor = c;
		text.setFillColor(textColor);
	}
	inline void SetTextString(std::string s) {
		text.setString(s);
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	}	
	inline void SetFont(sf::Font* newFont) {
		if (newFont != nullptr) {
			font = newFont;
			text.setFont(*font);
		}
	}
	inline void CenterText() {
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
		text.setPosition(sf::Vector2f(position.x + (size.x / 2), position.y + (size.y / 2.f) - 4));
	}
	virtual void OffsetText(float x, float y) { text.setPosition(text.getPosition().x + x, text.getPosition().y + y); }

	//get 
	inline std::string GetText() { return text.getString(); }
	virtual sf::Vector2f GetSize() { return size; }
	virtual sf::Vector2f GetPosition() { return position; }

	//set functionality
	inline virtual bool IsHovering() { return isHovering; }
	inline void EnableClick() { clickable = true; }
	inline void DisableClick() { clickable = false; }
	inline void EnableHover() { hoverable = true; }
	inline void DisableHover() { hoverable = false; }

	//toggle show/hide
	inline void Show() { hidden = false; }
	inline void Hide() { hidden = true; }	
			
	virtual void operator=(UIElement b);
};

