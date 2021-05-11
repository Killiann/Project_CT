#include "Button.h"

Button::Button() : UIElement(){}

Button::Button(sf::Vector2f btnPosition, sf::Vector2f btnSize, ResourceManager* resource, std::string btnText, void (f)(void*) ,void* ctx)
	: UIElement(btnPosition, btnSize, resource), context(ctx), func(f){	

	//defaults
	SetColor(sf::Color(40, 40, 40)); 
	SetOutlineThickness(0.f);
	SetTextColor(sf::Color::White);
	SetHoverColor(sf::Color(10,10,10));
	SetClickColor(sf::Color(100, 100, 100));

	SetTextString(btnText);
}

void Button::OnClick() {
	UIElement::OnClick();
	func(context);
}

void Button::operator=(Button b) {
	UIElement::operator=(b);

	this->func = b.func;
	this->context = b.context;
}