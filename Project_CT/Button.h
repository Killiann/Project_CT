#pragma once
#include "UIElement.h"

class Button : public UIElement
{
	//template <class T>
	void* context = nullptr;
	void (*func)(void*) = nullptr;	

	void OnClick();
public:
	Button(); //default
	Button(sf::Vector2f btnPosition, sf::Vector2f btnSize,  ResourceManager* resource, std::string btnText, void(f)(void*), void* ctx);

	void operator=(Button b);
};

