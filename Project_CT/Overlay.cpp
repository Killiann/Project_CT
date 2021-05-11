#include "Overlay.h"

//nav
void ForwardOptions(void* overlay) {
	((Overlay*)overlay)->Switch(NavItem::Options);
}
void ForwardData(void* overlay) {
	((Overlay*)overlay)->Switch(NavItem::Data);
}

//options
void ForwardNextGen(void* overlay) {
	((Overlay*)overlay)->NextGen();
}
void ForwardRestartSim(void* overlay) {
	((Overlay*)overlay)->RestartSim();
}
void ForwardShowHide(void* overlay) {
	((Overlay*)overlay)->ShowHide();
}
void ForwardExport(void* overlay) {
	((Overlay*)overlay)->ExportToCSV();
}
void ForwardMainMenu(void* overlay) {
	((Overlay*)overlay)->OpenMainMenu();
}
void ForwardSave(void* overlay) {
	((Overlay*)overlay)->SaveSim();
}
void ForwardOpExit(void* overlay) {
	((Overlay*)overlay)->ExitSim();
}

Overlay::Overlay(ResourceManager* resource, Trainer* t, MainMenu* menu) : resourceManager(resource), trainer(t), mainMenu(menu){
	background.setPosition(sf::Vector2f(position.x, position.y + navSize.y));
	background.setSize(size);
	background.setFillColor(backgroundColor);

	Button btn_settings = Button(position, navSize, resourceManager, "Options", ForwardOptions, this);
	Button btn_data = Button(sf::Vector2f(position.x + navSize.x, position.y), navSize, resourceManager, "Data", ForwardData, this);
	navElements.emplace("btn_settings", std::make_shared<Button>(btn_settings));
	navElements.emplace("btn_data", std::make_shared<Button>(btn_data));

	InitOptions();
	InitData();
}

void Overlay::InitOptions() {
	//prompt
	Label lbl_prompt = Label(sf::Vector2f(position.x + padding + 150 + 30, position.y + navSize.y + padding), sf::Vector2f(250, 100), resourceManager, "", 0.5);
	lbl_prompt.SetFont(resourceManager->GetRobotoLight());
	optionElements.emplace("lbl_prompt", std::make_shared<Label>(lbl_prompt));

	//shortcut key
	Label lbl_shortcut = Label(sf::Vector2f(position.x + size.x - 100 - padding, position.y + navSize.y + padding + btnMargin *6), sf::Vector2f(100, 30), resourceManager, "", 0.5);
	lbl_shortcut.SetFont(resourceManager->GetRobotoRegular());
	lbl_shortcut.SetTextColor(sf::Color(150, 150, 150));
	optionElements.emplace("lbl_shortcut", std::make_shared<Label>(lbl_shortcut));

	//error/success info label
	Label lbl_saveSuccess = Label(sf::Vector2f(position.x + padding + 150 + 30, position.y + navSize.y + padding + btnMargin * 3), sf::Vector2f(250, 100), resourceManager, "", 0.5);
	lbl_saveSuccess.SetFont(resourceManager->GetRobotoRegular());
	lbl_saveSuccess.SetTextColor(sf::Color::Green);
	lbl_saveSuccess.Hide();
	optionElements.emplace("lbl_saveSuccess", std::make_shared<Label>(lbl_saveSuccess));

	//skip to next generation
	Button btn_nextGen = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y), sf::Vector2f(150, 30), resourceManager, "Next Gen", ForwardNextGen, this);
	optionElements.emplace("btn_nextGen", std::make_shared<Button>(btn_nextGen));
	
	//restart with same settings
	Button btn_restart = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + btnMargin), sf::Vector2f(150, 30), resourceManager, "Restart", ForwardRestartSim, this);
	optionElements.emplace("btn_restart", std::make_shared<Button>(btn_restart));
	
	//show/hide overlay
	Button btn_showHide = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + btnMargin * 2), sf::Vector2f(150, 30), resourceManager, "Show Overlay", ForwardShowHide, this);
	optionElements.emplace("btn_showHide", std::make_shared<Button>(btn_showHide));

	//export to CSV
	Button btn_export = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + btnMargin * 3), sf::Vector2f(150, 30), resourceManager, "Export to CSV", ForwardExport, this);
	optionElements.emplace("btn_export", std::make_shared<Button>(btn_export));

	//save
	Button btn_save = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + btnMargin * 4), sf::Vector2f(150, 30), resourceManager, "Save", ForwardSave, this);
	optionElements.emplace("btn_save", std::make_shared<Button>(btn_save));

	//main menu button	
	Button btn_mainMenu = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + btnMargin * 5), sf::Vector2f(150, 30), resourceManager, "Main Menu", ForwardMainMenu, this);
	optionElements.emplace("btn_mainMenu", std::make_shared<Button>(btn_mainMenu));	

	//Exit
	Button btn_exit = Button(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + btnMargin * 6), sf::Vector2f(150, 30), resourceManager, "Exit", ForwardOpExit, this);
	optionElements.emplace("btn_exit", std::make_shared<Button>(btn_exit));
}

void Overlay::InitData() {
	//fps
	Label lbl_fps = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "FPS: ", 0.5f);
	Label lbl_fps_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_fps", std::make_shared<Label>(lbl_fps));
	dataElements.emplace("lbl_fps_val", std::make_shared<Label>(lbl_fps_val));

	//generation size
	Label lbl_genSize = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Generation Size: ", 0.5f);
	Label lbl_genSize_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_genSize", std::make_shared<Label>(lbl_genSize));
	dataElements.emplace("lbl_genSize_val", std::make_shared<Label>(lbl_genSize_val));

	//generation number
	Label lbl_currentGen = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 2), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Current Generation: ", 0.5f);
	Label lbl_currentGen_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 2), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_currentGen", std::make_shared<Label>(lbl_currentGen));
	dataElements.emplace("lbl_currentGen_val", std::make_shared<Label>(lbl_currentGen_val));

	//best lap time
	Label lbl_lapTime = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 3), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Best Lap Time: ", 0.5f);
	Label lbl_lapTime_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 3), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_lapTime", std::make_shared<Label>(lbl_lapTime));
	dataElements.emplace("lbl_lapTime_val", std::make_shared<Label>(lbl_lapTime_val));

	//best lap time(previous generation)
	Label lbl_lapTimePrev = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 4), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Best Lap time (prev gen): ", 0.5f);
	Label lbl_lapTimePrev_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 4), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_lapTimePrev", std::make_shared<Label>(lbl_lapTimePrev));
	dataElements.emplace("lbl_lapTimePrev_val", std::make_shared<Label>(lbl_lapTimePrev_val));

	//best fitness
	Label lbl_fitness = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 5), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Best Fitness: ", 0.5f);
	Label lbl_fitness_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 5), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_fitness", std::make_shared<Label>(lbl_fitness));
	dataElements.emplace("lbl_fitness_val", std::make_shared<Label>(lbl_fitness_val));

	//best fitness(previous generation
	Label lbl_fitnessPrev = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 6), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Best Fitness (prev gen): ", 0.5f);
	Label lbl_fitnessPrev_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 6), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_fitnessPrev", std::make_shared<Label>(lbl_fitnessPrev));
	dataElements.emplace("lbl_fitnessPrev_val", std::make_shared<Label>(lbl_fitnessPrev_val));

	//current generation time
	Label lbl_currentTime = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin *7), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Current Gen Time: ", 0.5f);
	Label lbl_currentTime_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 7), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_currentTime", std::make_shared<Label>(lbl_currentTime));
	dataElements.emplace("lbl_currentTime_val", std::make_shared<Label>(lbl_currentTime_val));
	
	//total time
	Label lbl_totalTime = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 8), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Total Run Time: ", 0.5f);
	Label lbl_totalTime_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 8), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_totalTime", std::make_shared<Label>(lbl_totalTime));
	dataElements.emplace("lbl_totalTime_val", std::make_shared<Label>(lbl_totalTime_val));

	//hl activation function
	Label lbl_hlActivation = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 9), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Hidden Layer Activation Function: ", 0.5f);
	Label lbl_hlActivation_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 9), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_hlActivation", std::make_shared<Label>(lbl_hlActivation));
	dataElements.emplace("lbl_hlActivation_val", std::make_shared<Label>(lbl_hlActivation_val));

	//output activation function
	Label lbl_olActivation = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 10), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Output  Layer Activation Function: ", 0.5f);
	Label lbl_olActivation_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 10), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_olActivation", std::make_shared<Label>(lbl_olActivation));
	dataElements.emplace("lbl_olActivation_val", std::make_shared<Label>(lbl_olActivation_val));

	//mutation rate
	Label lbl_mutationRate = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 11), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Mutation Rate: ", 0.5f);
	Label lbl_mutationRate_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 11), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_mutationRate", std::make_shared<Label>(lbl_mutationRate));
	dataElements.emplace("lbl_mutationRate_val", std::make_shared<Label>(lbl_mutationRate_val));

	//mutation rate (min)
	Label lbl_mutationRateMin = Label(sf::Vector2f(position.x + padding, position.y + padding + navSize.y + dataRowMargin * 12), sf::Vector2f(dataLeftColW, dataRowHeight), resourceManager, "Mutation Rate (minor): ", 0.5f);
	Label lbl_mutationRateMin_val = Label(sf::Vector2f(position.x + padding + dataLeftColW, position.y + padding + navSize.y + dataRowMargin * 12), sf::Vector2f(dataRowHeight, dataRowHeight), resourceManager, "", 0.5f);
	dataElements.emplace("lbl_mutationRateMin", std::make_shared<Label>(lbl_mutationRateMin));
	dataElements.emplace("lbl_mutationRateMin_val", std::make_shared<Label>(lbl_mutationRateMin_val));
}

void Overlay::Update(sf::RenderWindow& window, sf::Event &event) {
	if (exit) window.close();
	//keep nav updating
	for (auto& e : navElements)
		e.second->Update(window, event);

	//update options if open
	if (currentState == NavItem::Options)
		UpdateOptions(window, event);
}

//helper
int GetStringIndex(std::string s, std::vector<std::string> v) {
	for (unsigned int i = 0; i < v.size(); ++i) 
		if (v[i] == s) return i;
	return 0;
}

void Overlay::UpdateOptions(sf::RenderWindow& window, sf::Event& event) {
	for (auto& e : optionElements)
		e.second->Update(window, event);

	bool hovering = false;
	for (auto& e : optionElements) 
		if (e.second->IsHovering()) {
			(*optionElements.find("lbl_prompt")).second->SetText(promptMessages[GetStringIndex(e.first, optionButtonIDS)]);
			//shortcut popup
			if(e.first != "btn_exit")
				(*optionElements.find("lbl_shortcut")).second->SetText(shortcutMessages[GetStringIndex(e.first, optionButtonIDS)]);
			hovering = true;
		}		
	if (!hovering) {
		(*optionElements.find("lbl_shortcut")).second->SetText("");
		(*optionElements.find("lbl_prompt")).second->SetText("");
	}
}

//gets called externally to fix timing issues with Event call in main loop
void Overlay::UpdateData(std::string fps) {
	if (trainer->IsRunning() && currentState == NavItem::Data) {
		TrainerData data = trainer->GetData();
		(*(dataElements).find("lbl_fps_val")).second->SetText(fps);
		(*(dataElements).find("lbl_genSize_val")).second->SetText(std::to_string(data.generationSize));
		(*(dataElements).find("lbl_currentGen_val")).second->SetText(std::to_string(data.currentGeneration));
		(*(dataElements).find("lbl_lapTime_val")).second->SetText(FloatToTime(data.bestLapTime));
		(*(dataElements).find("lbl_lapTimePrev_val")).second->SetText(FloatToTime(data.bestLapTimePrev));
		(*(dataElements).find("lbl_fitness_val")).second->SetText(TruncateFloat(data.bestFitness));
		(*(dataElements).find("lbl_fitnessPrev_val")).second->SetText(TruncateFloat(data.bestFitnessPrev));
		(*(dataElements).find("lbl_currentTime_val")).second->SetText(FloatToTime(data.currentTime));
		(*(dataElements).find("lbl_totalTime_val")).second->SetText(FloatToTime(data.totalTime));
		(*(dataElements).find("lbl_hlActivation_val")).second->SetText(data.hlActivation);
		(*(dataElements).find("lbl_olActivation_val")).second->SetText(data.olActivation);
		(*(dataElements).find("lbl_mutationRate_val")).second->SetText(TruncateFloat(data.mutationRate) + "%");
		(*(dataElements).find("lbl_mutationRateMin_val")).second->SetText(TruncateFloat(data.mutationRateMinor) + "%");
	}
}

void Overlay::Draw(sf::RenderTarget &window) {
	if(currentState != NavItem::None)
		window.draw(background);

	for (auto& e : navElements)
		e.second->Draw(window);

	switch (currentState) {
	case(NavItem::Options):
		for (auto& e : optionElements)
			e.second->Draw(window); break;

	case(NavItem::Data):
		for (auto& e : dataElements)
			e.second->Draw(window); break;
	}
}

void Overlay::SetNavColor() {
	(*navElements.find("btn_data")).second->SetColor(sf::Color(40, 40, 40));
	(*navElements.find("btn_settings")).second->SetColor(sf::Color(40, 40, 40));
	switch (currentState) {
	case(NavItem::Options): {
		(*navElements.find("btn_settings")).second->SetColor(selectedColor);
	}break;
	case(NavItem::Data) :{
		(*navElements.find("btn_data")).second->SetColor(selectedColor);
		}break;
	}
}