#pragma once
#include "UIElement.h"

class Textbox : public UIElement 
{
	bool numeric = false;
	bool isFocused = false;

	unsigned  int max = 0;
		
	std::string value;

	//blinking cursor
	float cTime = 500.f; //milliseconds
	bool showCursor = false;
	sf::Clock cursorTimer;	

	void OnClick();

	inline void Focused(bool focused) {
		isFocused = focused;
		if (isFocused) {
			cursorTimer.restart();
			SetOutlineThickness(2.f);
		}
		else {
			SetOutlineThickness(1.f);
			if (value != "") {
				if (numeric && std::stoi(value) > max) {
					value = std::to_string(max);
					text.setString(value);
				}
			}
		}
	}

public:
	Textbox(){}
	Textbox(sf::Vector2f position, sf::Vector2f size, ResourceManager* resource, bool numeric, int max);

	void Update(sf::RenderWindow& window, sf::Event& event);	
	void Draw(sf::RenderTarget& window);

	void operator=(Textbox t);
};

