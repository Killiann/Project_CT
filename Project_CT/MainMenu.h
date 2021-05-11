#pragma once
#include <SFML/Graphics.hpp>
#include <thread>

#include "ResourceManager.h"
#include "Button.h"
#include "Textbox.h"
#include "Dropdown.h"
#include "Label.h"
#include "UIElement.h"
#include "Trainer.h"

class MainMenu
{
	ResourceManager* resourceManager;
	Trainer* trainer;

	std::vector<std::string> activationFuncs{ "Sigmoid", "Leaky RELU", "Binary Step", "Tanh" };

	enum class MenuState {
		Navigation,
		NewSimulation,
	};

	MenuState currentState = MenuState::Navigation;

	//limits
	const unsigned int maxHiddenLayers = 4;
	const unsigned int minHiddenLayers = 1;
	const unsigned int maxCarsPerThread = 50;
	const unsigned int maxNodes = 10;
	unsigned int maxThreads = 1;
	std::vector<int> hiddenLayerData;
	
	//setting inputs
	unsigned int hiddenLayers = 1;
	unsigned int threadCount = 1;
	unsigned int carsPerThread = 50;
	unsigned int generationSize = (1 + threadCount) * carsPerThread;
	unsigned int hiddenFuncID = 0;
	unsigned int outputFuncID = 0;
	float mutationRate = 0.01f;
	float mutationMinRate = 0.05f;

	//helpers
	bool exit = false;
	bool hidden = false;
	bool withContinue = false;

	//styling
	unsigned int padding = 35;
	unsigned int marginTop = 70;
	unsigned int margin = 30;
	unsigned int rowHeight = 25;

	//middle column
	unsigned int marginLeft = 285;

	//settings
	unsigned int leftColW = 200;
	unsigned int rightColW = 30;
	unsigned int settingsMargin = 26;
	unsigned int rowH = 26;
	unsigned int s_marginTop = marginTop + padding + (unsigned)position.y;
	unsigned int s_marginLeft = 570;

	//SFML
	sf::Vector2f size = sf::Vector2f(900.f, 500.f);
	sf::Vector2f position = sf::Vector2f(300.f, 200.f);	
	sf::RectangleShape background;
	sf::Text title;
	
	//UI elements
	std::map <std::string, std::shared_ptr<UIElement>> newSimulationElements;
	std::map <std::string, std::shared_ptr<UIElement>> navigationElements;

	//initialisation
	void InitializeNavigation();
	void InitializeNewSimulation();
	void InitalizeSettings();

	//update functions per state
	void NavigationState(sf::RenderWindow& window, sf::Event& event);
	void NewSimulationState(sf::RenderWindow& window, sf::Event& event);

	//update functions
	void UpdateSettings();
	inline void UpdateHLElements() {
		for (unsigned int i = 0; i < maxHiddenLayers; ++i) {
			(*newSimulationElements.find("lbl_layerCount_" + std::to_string(i))).second->Hide();
			(*newSimulationElements.find("txt_layerCount_" + std::to_string(i))).second->Hide();
		}
		for (unsigned int i = 0; i < hiddenLayers; ++i) {
			(*newSimulationElements.find("lbl_layerCount_" + std::to_string(i))).second->Show();
			(*newSimulationElements.find("txt_layerCount_" + std::to_string(i))).second->Show();
		}
	}

public:
	MainMenu(ResourceManager* resource, Trainer* trainer);
	void Update(sf::RenderWindow& window, sf::Event& event);
	void Draw(sf::RenderTarget& window);	

	void Hide();
	void Show(bool withContinue = false);

	//button actions
	inline void NewSim() { 
		currentState = MenuState::NewSimulation; 
		title.setString("Create new Trainer");
	}
	inline void LoadSim() {
		if (trainer->LoadScene("trainer.sim")) {
			Hide();
		}
		else {
			(*navigationElements.find("lbl_error")).second->SetText("Could not load file.");
			(*navigationElements.find("lbl_error")).second->Show();
		}
	}
	inline void ExitApp() { exit = true; }
	inline void CreateNewSim() { 
		std::vector<int> hlVec = hiddenLayerData;
		hlVec.resize(hiddenLayers);
		trainer->SetupTrainer(threadCount, carsPerThread, hlVec, hiddenFuncID, outputFuncID, mutationRate, mutationMinRate);
		Hide();
	}
	inline void ContinueSim() { 
		trainer->Continue(); 
		Hide();
	}

	//add/remove layers on simulation setup screen
	inline void AddHiddenLayer() { 
		if (hiddenLayers < maxHiddenLayers) ++hiddenLayers; 
		(*newSimulationElements.find("lbl_hiddenLayers")).second->SetTextString("Hidden Layers: " + std::to_string(hiddenLayers));
		UpdateHLElements();
	}
	inline void SubHiddenLayer() {
		if (hiddenLayers > minHiddenLayers) --hiddenLayers;
		(*newSimulationElements.find("lbl_hiddenLayers")).second->SetTextString("Hidden Layers: " + std::to_string(hiddenLayers));
		UpdateHLElements();
 	}	

	//shared
	inline void Back() { 
		currentState = MenuState::Navigation; 
		title.setString("Car Controller Neural Net Trainer");
	}
};

