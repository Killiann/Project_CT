#pragma once
#include <iomanip>
#include <sstream>
#include "UIElement.h"
#include "Trainer.h"
#include "Button.h"
#include "Label.h"
#include "MainMenu.h"

enum class NavItem {
	None,
	Options,
	Data
};

class Overlay
{
	//prompt info
	std::vector<std::string> optionButtonIDS{ "btn_nextGen", "btn_restart", "btn_showHide", "btn_export", "btn_save", "btn_mainMenu", "btn_exit" };
	std::vector<std::string> promptMessages = {
		"Skip to next generation",
		"Restart simulation without saving",
		"Toggle overlay",
		"Export data to CSV file",
		"Save simulation",
		"Go to Main Menu",
		"Exit application"
	};
	std::vector<std::string> shortcutMessages = {
		"Shortcut: 1",
		"Shortcut: 0",
		"Shortcut: C",
		"Shortcut: X",
		"Shortcut: S",
		"Shortcut: ESC",
		"Exit application"
	};

	//state
	NavItem currentState = NavItem::None;
	bool exit = false;
	bool devOverlay = false;

	//ptr
	ResourceManager* resourceManager = nullptr;
	Trainer* trainer = nullptr;
	MainMenu* mainMenu = nullptr;

	//styling
	unsigned int padding = 20;
	unsigned int btnMargin = 40;
	unsigned int dataLeftColW = 350;
	unsigned int dataRowHeight = 15;
	unsigned int dataRowMargin = 20;
	const sf::Vector2f navSize = sf::Vector2f(150, 30);
	const sf::Vector2f position = sf::Vector2f(0, 0);
	const sf::Vector2f size = sf::Vector2f(500, 320);
	
	sf::RectangleShape background;
	sf::Color backgroundColor = sf::Color(255, 255, 255);
	sf::Color selectedColor = sf::Color(80, 80, 80);

	//UI Elements
	std::map<std::string, std::shared_ptr<UIElement>> navElements;
	std::map<std::string, std::shared_ptr<UIElement>> optionElements;
	std::map<std::string, std::shared_ptr<UIElement>> dataElements;

	void InitOptions();
	void InitData();
	void UpdateOptions(sf::RenderWindow& window, sf::Event& event);
	void SetNavColor();
	
public:
	Overlay() {}
	Overlay(ResourceManager* resource, Trainer* t, MainMenu* menu);
	void Update(sf::RenderWindow& window, sf::Event& event);
	void UpdateData(std::string fps);
	void Draw(sf::RenderTarget& window);
	inline bool IsDevOn() { return devOverlay; }

	//toggle navigation
	inline void Switch(NavItem screen) {
		(*optionElements.find("lbl_saveSuccess")).second->Hide();
		if (currentState == screen) currentState = NavItem::None;
		else currentState = screen;
		SetNavColor();
	}

	//button functions
	inline void NextGen() { trainer->NextGeneration(false); }
	inline void RestartSim() { trainer->ResetScene(); }
	inline void ShowHide() {
		if (devOverlay) {
			devOverlay = false;
			(*optionElements.find("btn_showHide")).second->SetText("Show Overlay");
		}
		else {
			devOverlay = true;
			(*optionElements.find("btn_showHide")).second->SetText("Hide Overlay");
		}
		(*optionElements.find("btn_showHide")).second->CenterText();
	}
	inline void ExportToCSV() { 		
		if (trainer->ExportData("data.csv")) {
			(*optionElements.find("lbl_saveSuccess")).second->SetTextColor(sf::Color(33, 158, 44));
			(*optionElements.find("lbl_saveSuccess")).second->SetText("File Exported");
			(*optionElements.find("lbl_saveSuccess")).second->Show();
		}
		else {
			(*optionElements.find("lbl_saveSuccess")).second->SetTextColor(sf::Color::Red);
			(*optionElements.find("lbl_saveSuccess")).second->SetText("Error exporting file. Check Console.");
			(*optionElements.find("lbl_saveSuccess")).second->Show();
		}
	}
	inline void SaveSim() {
		if (trainer->SaveScene("trainer.sim")) {
			(*optionElements.find("lbl_saveSuccess")).second->SetTextColor(sf::Color(33, 158, 44));
			(*optionElements.find("lbl_saveSuccess")).second->SetText("File Saved");
			(*optionElements.find("lbl_saveSuccess")).second->Show();
		}
		else {
			(*optionElements.find("lbl_saveSuccess")).second->SetTextColor(sf::Color::Red);
			(*optionElements.find("lbl_saveSuccess")).second->SetText("Error saving file. Check Console.");
			(*optionElements.find("lbl_saveSuccess")).second->Show();
		}
	}
	inline void OpenMainMenu() {
		currentState = NavItem::None;
		trainer->Pause();
		mainMenu->Show(true);
	}
	inline void ExitSim() { exit = true; }
};

