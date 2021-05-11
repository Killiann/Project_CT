#include "Dropdown.h"

Dropdown::Dropdown(sf::Vector2f ddPosition, sf::Vector2f ddSize, ResourceManager* resource, std::string ddText)
	: UIElement(ddPosition, ddSize, resource) {
	//dropdown arrow
	arrow.setTexture(*resourceManager->GetArrowTexture());
	arrow.setScale(0.3, 0.3);
	arrow.setOrigin(23, 12);
	arrow.setPosition(position.x + size.x - (size.y / 2), position.y + (size.y / 2));

	//defaults
	SetColor(sf::Color(40, 40, 40));
	SetOutlineThickness(0.f);
	SetTextColor(sf::Color::White);
	SetHoverColor(sf::Color(10, 10, 10));
	SetClickColor(sf::Color(100, 100, 100));

	SetTextString(ddText);
}

void Dropdown::AddItem(std::string itemName) {
	sf::Vector2f itemSize = sf::Vector2f(size.x - 2.f, size.y - 10);
	sf::Vector2f pos(background.getPosition().x + 1.f, (background.getPosition().y + size.y + ((items.size()) * (itemSize.y + 2.f))));
	DropdownItem newItem(pos, itemSize, resourceManager, itemName);
	items.push_back(newItem);
}

void Dropdown::OnClick() {
	isOpen = isOpen ? false : true;
	arrow.rotate(180);
}

void Dropdown::Update(sf::RenderWindow& window, sf::Event& event) {
	UIElement::Update(window, event);
	
	sf::FloatRect bounds = background.getGlobalBounds();
	
	if (isOpen) {
		//close if clicked out of Dropdown
		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				if (!bounds.contains((sf::Vector2f)sf::Mouse::getPosition(window))) {
					bool isContained = false;
					for (auto& i : items)
						if (i.background.getGlobalBounds().contains((sf::Vector2f)sf::Mouse::getPosition(window)))
							isContained = true;

					if (!isContained) {
						OnClick();
					}
				}
			}
		}

		for (unsigned int i = 0; i < items.size(); ++i) {
			bool selected = items[i].selected; //check if selected before update
			
			items[i].Update(window, event);
			
			if (!selected) { //if has been selected in this loop
				if (items[i].selected) {
					for (unsigned int j = 0; j < items.size(); ++j) {
						if (i != j) {
							items[j].Selected(false);
						}
					}
					selectedIndex = i;
					OnClick();
				}
			}
		}
	}
}

void Dropdown::Draw(sf::RenderTarget& window) {	
	UIElement::Draw(window);

	window.draw(arrow);
	if (isOpen) {
		for (auto& i : items)
			i.Draw(window);
	}
}

void Dropdown::operator=(Dropdown dd) {
	UIElement::operator=(dd);
	
	isOpen = dd.isOpen;
	selectedIndex = dd.selectedIndex;
	arrow = dd.arrow;
	items = dd.items;
}

//dropdown item -------------

DropdownItem::DropdownItem(sf::Vector2f position, sf::Vector2f size, ResourceManager* resource, std::string ddText)
	: UIElement(position, size, resource) {

	//defaults
	SetColor(sf::Color(80, 80, 80));
	SetOutlineThickness(0.f);
	SetTextColor(sf::Color::White);
	SetHoverColor(sf::Color(50, 50, 50));
	SetClickColor(sf::Color(120, 120, 120));
	SetAccentColor(sf::Color::White);
	SetOutlineThickness(1.f);
	SetTextString(ddText);

	background.setOutlineColor(color);
}

void DropdownItem::operator=(DropdownItem i) {
	UIElement::operator=(i);
	selected = i.selected;
}

void DropdownItem::OnClick() {
	selected ? Selected(false) : Selected(true);
}
