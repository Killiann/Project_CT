#include "MainMenu.h"

//back (Shared)
void ForwardBack(void* menu) { ((MainMenu*)menu)->Back(); }

//navigation
void ForwardContinueCurrent(void* menu) { ((MainMenu*)menu)->ContinueSim(); }
void ForwardNewSim(void* menu) { ((MainMenu*)menu)->NewSim(); }
void ForwardLoadSim(void* menu) { ((MainMenu*)menu)->LoadSim(); }
void ForwardSimExit(void* menu) { ((MainMenu*)menu)->ExitApp(); }

//new simulation
void ForwardSubLayer(void* menu) { ((MainMenu*)menu)->SubHiddenLayer(); }
void ForwardAddLayer(void* menu) { ((MainMenu*)menu)->AddHiddenLayer(); }
void ForwardContinue(void* menu) { ((MainMenu*)menu)->CreateNewSim(); }

MainMenu::MainMenu(ResourceManager *resource, Trainer* t): resourceManager(resource), trainer(t) {
	//find max available threads in system
	maxThreads = std::thread::hardware_concurrency();

	background.setPosition(position);
	background.setSize(size);
	background.setFillColor(sf::Color::White);

	//Menu title
	title.setPosition(position.x + padding, position.y + padding);
	title.setString("Car Controller Neural Net Trainer");
	title.setFont(*resource->GetRobotoLight());
	title.setFillColor(sf::Color(60, 60, 60));		

	//setup hidden layer data
	hiddenLayerData.assign(maxHiddenLayers, 1);
	
	//initialize pages
	s_marginTop = marginTop + padding + position.y; // styling needs updating with new pos
	InitializeNavigation();
	InitializeNewSimulation();
}

//initialise=================
void MainMenu::InitializeNavigation() {
	//navigation 
	float topPadding = 50;
	if (withContinue) topPadding = 100;

	Button btn_continue = Button(sf::Vector2f(position.x + padding, position.y + padding + topPadding), sf::Vector2f(150, 40), resourceManager, "Continue", ForwardContinueCurrent, this);
	Button btn_newSim = Button(sf::Vector2f(position.x + padding, position.y + padding + topPadding + 50), sf::Vector2f(150, 40), resourceManager, "New", ForwardNewSim, this);
	Button btn_loadSim = Button(sf::Vector2f(position.x + padding, position.y + padding + topPadding + 100), sf::Vector2f(150, 40), resourceManager, "Load", ForwardLoadSim, this);
	Button btn_exitSim = Button(sf::Vector2f(position.x + padding, position.y + padding + topPadding + 150), sf::Vector2f(150, 40), resourceManager, "Exit", ForwardSimExit, this);

	sf::Vector2f promptPos(position.x + btn_newSim.GetSize().x + (padding * 2), position.y + padding + 100);
	Label lbl_prompt = Label(promptPos, sf::Vector2f(0, 0), resourceManager, "", 0.55);
	lbl_prompt.SetFont(resourceManager->GetRobotoLight());

	//load error
	sf::Vector2f errorPromptPos(position.x + btn_newSim.GetSize().x + (padding * 2), position.y + padding + 150);
	Label lbl_error = Label(errorPromptPos, sf::Vector2f(0, 0), resourceManager, "", 0.55);
	lbl_error.SetColor(sf::Color::Red);
	lbl_error.SetFont(resourceManager->GetRobotoRegular());
	lbl_error.Hide();

	navigationElements.emplace("lbl_prompt", std::make_shared<Label>(lbl_prompt));
	navigationElements.emplace("lbl_error", std::make_shared<Label>(lbl_error));

	if(withContinue) navigationElements.emplace("btn_continueSim", std::make_shared<Button>(btn_continue));
	navigationElements.emplace("btn_newSim", std::make_shared<Button>(btn_newSim));
	navigationElements.emplace("btn_loadSim", std::make_shared<Button>(btn_loadSim));
	navigationElements.emplace("btn_exitSim", std::make_shared<Button>(btn_exitSim));
}

void MainMenu::InitializeNewSimulation() {
	//threads used
	Label lbl_performance = Label(sf::Vector2f(position.x + padding, position.y + padding + marginTop), sf::Vector2f(190, rowHeight), resourceManager, "Performance:", 0.6);
	Label lbl_threadCount = Label(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin), sf::Vector2f(190, rowHeight), resourceManager, "Helper threads (max:" + std::to_string(maxThreads) + "):", 0.5f);
	Textbox txt_threadCount = Textbox(sf::Vector2f(position.x + padding + 210, position.y + padding + marginTop + margin), sf::Vector2f(28, rowHeight), resourceManager, true, maxThreads);
	newSimulationElements.emplace("lbl_performance", std::make_shared<Label>(lbl_performance));
	newSimulationElements.emplace("lbl_threadCount", std::make_shared<Label>(lbl_threadCount));
	newSimulationElements.emplace("txt_threadCount", std::make_shared<Textbox>(txt_threadCount));

	//cars per thread
	Label lbl_carsPerThread = Label(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin * 2), sf::Vector2f(190, rowHeight), resourceManager, "Cars per thread (max:" + std::to_string(maxCarsPerThread) + "):", 0.5f);
	Textbox txt_carsPerThread = Textbox(sf::Vector2f(position.x + padding + 210, position.y + padding + marginTop + margin * 2), sf::Vector2f(28, rowHeight), resourceManager, true, maxCarsPerThread);
	newSimulationElements.emplace("lbl_carsPerThread", std::make_shared<Label>(lbl_carsPerThread));
	newSimulationElements.emplace("txt_carsPerThread", std::make_shared<Textbox>(txt_carsPerThread));

	//mutation rate
	Label lbl_mutationRate = Label(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin * 3), sf::Vector2f(190, rowHeight), resourceManager, "Mutation rate:(n/100)%", 0.5f);
	Textbox txt_mutationRate = Textbox(sf::Vector2f(position.x + padding + 210, position.y + padding + marginTop + margin * 3), sf::Vector2f(42, rowHeight), resourceManager, true, 9999);
	newSimulationElements.emplace("lbl_mutationRate", std::make_shared<Label>(lbl_mutationRate));
	newSimulationElements.emplace("txt_mutationRate", std::make_shared<Textbox>(txt_mutationRate));

	//mutation rate (lesser)
	Label lbl_mutationRateMin = Label(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin * 4), sf::Vector2f(190, rowHeight), resourceManager, "Minor mutation rate:(n/100)%", 0.5f);
	Textbox txt_mutationRateMin = Textbox(sf::Vector2f(position.x + padding + 210, position.y + padding + marginTop + margin * 4), sf::Vector2f(42, rowHeight), resourceManager, true, 9999);
	newSimulationElements.emplace("lbl_mutationRateMin", std::make_shared<Label>(lbl_mutationRateMin));
	newSimulationElements.emplace("txt_mutationRateMin", std::make_shared<Textbox>(txt_mutationRateMin));

	//activation function selection - hidden layer
	Label lbl_activation = Label(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin * 6), sf::Vector2f(190, rowHeight), resourceManager, "Activation Functions:", 0.6);
	Dropdown dd_hiddenActivation = Dropdown(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin * 6), sf::Vector2f(190, 30), resourceManager, "Hidden Layers");
	dd_hiddenActivation.AddItem("Sigmoid");
	dd_hiddenActivation.AddItem("Leaky RELU");
	dd_hiddenActivation.AddItem("Binary Step");
	dd_hiddenActivation.AddItem("Tanh");

	//output layer
	Dropdown dd_outputActivation = Dropdown(sf::Vector2f(position.x + padding, position.y + padding + marginTop + margin * 7.2), sf::Vector2f(190, 30), resourceManager, "Output Layer");
	dd_outputActivation.AddItem("Sigmoid");
	dd_outputActivation.AddItem("Leaky RELU");
	dd_outputActivation.AddItem("Binary Step");
	dd_outputActivation.AddItem("Tanh");
	newSimulationElements.emplace("lbl_activation", std::make_shared<Label>(lbl_activation));
	newSimulationElements.emplace("dd_outputActivation", std::make_shared<Dropdown>(dd_outputActivation));
	newSimulationElements.emplace("dd_hiddenActivation", std::make_shared<Dropdown>(dd_hiddenActivation));

	//hidden layer setup
	Label lbl_hidden = Label(sf::Vector2f(position.x + padding + marginLeft, position.y + padding + marginTop), sf::Vector2f(190, rowHeight), resourceManager, "Hidden Layer Setup:", 0.6);
	Button btn_subLayer = Button(sf::Vector2f(position.x + padding + marginLeft, position.y + padding + marginTop + margin), sf::Vector2f(30, rowHeight), resourceManager, "-", ForwardSubLayer, this);
	Button btn_addLayer = Button(sf::Vector2f(position.x + padding + marginLeft + 210, position.y + padding + marginTop + margin), sf::Vector2f(30, rowHeight), resourceManager, "+", ForwardAddLayer, this);
	Label lbl_hiddenLayers = Label(sf::Vector2f(position.x + padding + marginLeft + 30, position.y + padding + marginTop + margin), sf::Vector2f(180, rowHeight), resourceManager, "Hidden Layers: " + std::to_string(hiddenLayers), 0.5);
	lbl_hiddenLayers.CenterText();
	btn_subLayer.SetFontScale(0.6);
	btn_subLayer.SetFontScale(0.6);
	btn_subLayer.OffsetText(0, -6);
	newSimulationElements.emplace("lbl_hidden", std::make_shared<Label>(lbl_hidden));
	newSimulationElements.emplace("btn_subLayer", std::make_shared<Button>(btn_subLayer));
	newSimulationElements.emplace("btn_addLyer", std::make_shared<Button>(btn_addLayer));
	newSimulationElements.emplace("lbl_hiddenLayers", std::make_shared<Label>(lbl_hiddenLayers));

	//for each available hidden layer
	for (unsigned int i = 0; i < maxHiddenLayers; ++i) {
		Label lbl_layerCount = Label(sf::Vector2f(position.x + padding + marginLeft, position.y + padding + marginTop + ((2 + i) * margin)), sf::Vector2f(190, rowHeight), resourceManager, "Nodes in layer " + std::to_string(i + 1) + ": (max: " + std::to_string(maxNodes) + ")", 0.5f);
		Textbox txt_layerCount = Textbox(sf::Vector2f(position.x + padding + marginLeft + 210, position.y + padding + marginTop + ((2 + i) * margin)), sf::Vector2f(30, rowHeight), resourceManager, true, maxNodes);
		newSimulationElements.emplace("lbl_layerCount_" + std::to_string(i), std::make_shared<Label>(lbl_layerCount));
		newSimulationElements.emplace("txt_layerCount_" + std::to_string(i), std::make_shared<Textbox>(txt_layerCount));
	}
	UpdateHLElements();

	//Continue Button
	Button btn_continue = Button(sf::Vector2f(position.x + size.x - 150 - padding, position.y + size.y - 40 - padding), sf::Vector2f(150, 40), resourceManager, "Continue", ForwardContinue, this);
	newSimulationElements.emplace("btn_continue", std::make_shared<Button>(btn_continue));

	//back button
	Button btn_back = Button(sf::Vector2f(position.x + padding, position.y + size.y - 40 - padding), sf::Vector2f(150, 40), resourceManager, "Back", ForwardBack, this);
	newSimulationElements.emplace("btn_back", std::make_shared<Button>(btn_back));
	
	//settings
	InitalizeSettings();
	UpdateSettings();
}

void MainMenu::InitalizeSettings() {
	//title
	Label lbl_settingsHeader = Label(sf::Vector2f(position.x + padding + s_marginLeft, position.y + padding + marginTop), sf::Vector2f(150, 30), resourceManager, "Settings:", 0.6);
	newSimulationElements.emplace("lbl_settingsHeader", std::make_shared<Label>(lbl_settingsHeader));

	//thread count
	Label lbl_settingsThreadCount = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 1)), sf::Vector2f(leftColW, rowH), resourceManager, "Thread Count: ", 0.5);
	Label lbl_settingsThreadCountVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 1)), sf::Vector2f(rightColW, rowH), resourceManager, std::to_string(threadCount), 0.5);
	newSimulationElements.emplace("lbl_settingsThreadCount", std::make_shared<Label>(lbl_settingsThreadCount));
	newSimulationElements.emplace("lbl_settingsThreadCountVal", std::make_shared<Label>(lbl_settingsThreadCountVal));

	//cars per thread
	Label lbl_settingsCarsPerThread = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 2)), sf::Vector2f(leftColW, rowH), resourceManager, "Cars per thread: ", 0.5);
	Label lbl_settingsCarsPerThreadVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 2)), sf::Vector2f(rightColW, rowH), resourceManager, std::to_string(carsPerThread), 0.5);
	newSimulationElements.emplace("lbl_settingsCarsPerThread", std::make_shared<Label>(lbl_settingsCarsPerThread));
	newSimulationElements.emplace("lbl_settingsCarsPerThreadVal", std::make_shared<Label>(lbl_settingsCarsPerThreadVal));

	//generation size
	Label lbl_settingsGenSize = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 3)), sf::Vector2f(leftColW, rowH), resourceManager, "Generation Size: ", 0.5);
	Label lbl_settingsGenSizeVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 3)), sf::Vector2f(rightColW, rowH), resourceManager, std::to_string(generationSize), 0.5);
	newSimulationElements.emplace("lbl_settingsGenSize", std::make_shared<Label>(lbl_settingsGenSize));
	newSimulationElements.emplace("lbl_settingsGenSizeVal", std::make_shared<Label>(lbl_settingsGenSizeVal));

	//generation size
	Label lbl_settingsMutationRate = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 4)), sf::Vector2f(leftColW, rowH), resourceManager, "Mutation Rate: ", 0.5);
	Label lbl_settingsMutationRateVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 4)), sf::Vector2f(rightColW, rowH), resourceManager, TruncateFloat(mutationRate) + "%", 0.5);
	newSimulationElements.emplace("lbl_settingsMutationRate", std::make_shared<Label>(lbl_settingsMutationRate));
	newSimulationElements.emplace("lbl_settingsMutationRateVal", std::make_shared<Label>(lbl_settingsMutationRateVal));

	//generation size
	Label lbl_settingsMinMutationRate = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 5)), sf::Vector2f(leftColW, rowH), resourceManager, "Mutation Rate(minor): ", 0.5);
	Label lbl_settingsMinMutationRateVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 5)), sf::Vector2f(rightColW, rowH), resourceManager, TruncateFloat(mutationMinRate) + "%", 0.5);
	newSimulationElements.emplace("lbl_settingsMinMutationRate", std::make_shared<Label>(lbl_settingsMinMutationRate));
	newSimulationElements.emplace("lbl_settingsMinMutationRateVal", std::make_shared<Label>(lbl_settingsMinMutationRateVal));

	//hidden layer function
	Label lbl_hlActivation = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 6)), sf::Vector2f(leftColW, rowH), resourceManager, "HL Activation Function:", 0.5);
	Label lbl_hlActivationVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 6)), sf::Vector2f(rightColW, rowH), resourceManager, activationFuncs[hiddenFuncID], 0.5);
	newSimulationElements.emplace("lbl_hlActivation", std::make_shared<Label>(lbl_hlActivation));
	newSimulationElements.emplace("lbl_hlActivationVal", std::make_shared<Label>(lbl_hlActivationVal));

	//output layer function
	Label lbl_olActivation = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * 7)), sf::Vector2f(leftColW, rowH), resourceManager, "OL Activation Function:", 0.5);
	Label lbl_olActivationVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * 7)), sf::Vector2f(rightColW, rowH), resourceManager, activationFuncs[outputFuncID], 0.5);
	newSimulationElements.emplace("lbl_olActivation", std::make_shared<Label>(lbl_olActivation));
	newSimulationElements.emplace("lbl_olActivationVal", std::make_shared<Label>(lbl_olActivationVal));

	//hidden layer
	for (unsigned int i = 0; i < maxHiddenLayers; ++i) {
		Label lbl_hiddenLayer = Label(sf::Vector2f(position.x + padding + s_marginLeft, s_marginTop + (settingsMargin * (8 + i))), sf::Vector2f(leftColW, rowH), resourceManager, "Hidden layer " + std::to_string(i + 1) + " nodes:", 0.5);
		Label lbl_hiddenLayerVal = Label(sf::Vector2f(position.x + padding + s_marginLeft + leftColW, s_marginTop + (settingsMargin * (8 + i))), sf::Vector2f(rightColW, rowH), resourceManager, std::to_string(hiddenLayerData[i]), 0.5);
		newSimulationElements.emplace("lbl_hiddenLayer_" + std::to_string(i), std::make_shared<Label>(lbl_hiddenLayer));
		newSimulationElements.emplace("lbl_hiddenLayerVal_" + std::to_string(i), std::make_shared<Label>(lbl_hiddenLayerVal));
	}
}

//update======================
void MainMenu::Update(sf::RenderWindow& window, sf::Event& event) {
	switch (currentState) {
	case(MenuState::Navigation): NavigationState(window, event); break;
	case(MenuState::NewSimulation): NewSimulationState(window, event); break;
	}
}

void MainMenu::NavigationState(sf::RenderWindow& window, sf::Event& event) {
	//main menu on load
	for (auto&m : navigationElements) {
		m.second->Update(window, event);
		int nameId = -1;
		if (m.first == "btn_newSim") nameId = 0;
		if (m.first == "btn_loadSim") nameId = 1;
		if (m.first == "btn_exitSim") nameId = 2;
		if (m.first == "btn_continueSim") nameId = 3;

		if (m.second->IsHovering()) {
			
			switch (nameId) {
				//new sim	
			case(0): (*navigationElements.find("lbl_prompt")).second->SetText("Start training a new model with custom parameters"); break;
				//load sim
			case(1):(*navigationElements.find("lbl_prompt")).second->SetText("Continue training the previously created model"); break;
				//exit application
			case(2): (*navigationElements.find("lbl_prompt")).second->SetText("Exit application"); break;
			case(3): (*navigationElements.find("lbl_prompt")).second->SetText("Continue"); break;
			default:break;
			}
		}
	}
	//close
	if (exit)
		window.close();
}

void MainMenu::NewSimulationState(sf::RenderWindow& window, sf::Event& event) {
	for (const auto& m : newSimulationElements)
		m.second->Update(window, event);
	UpdateSettings();
}

void MainMenu::UpdateSettings() {
	//get thread/ car count values (limit)
	std::string cpt_val = (*newSimulationElements.find("txt_carsPerThread")).second->GetText();
	std::string tc_val = (*newSimulationElements.find("txt_threadCount")).second->GetText();
	if (cpt_val != "") carsPerThread = std::min((unsigned)std::stoi(cpt_val), maxCarsPerThread);
	if (tc_val != "") threadCount = std::min((unsigned)std::stoi(tc_val), maxThreads);

	//get dropdown values
	Dropdown hiddenActivation = *(std::dynamic_pointer_cast<Dropdown> ((*newSimulationElements.find("dd_hiddenActivation")).second));
	Dropdown outputActivation = *(std::dynamic_pointer_cast<Dropdown> ((*newSimulationElements.find("dd_outputActivation")).second));
	hiddenFuncID = hiddenActivation.GetSelected();
	outputFuncID = outputActivation.GetSelected();

	generationSize = (threadCount + 1)* carsPerThread;

	//get mutation rate
	std::string mutationVal = (*newSimulationElements.find("txt_mutationRate")).second->GetText();
	std::string mutationMinVal = (*newSimulationElements.find("txt_mutationRateMin")).second->GetText();
	if (mutationVal != "") mutationRate = std::stof(mutationVal) / 100.f;
	if (mutationMinVal != "") mutationMinRate = std::stof(mutationMinVal) / 100.f;

	//update labels
	(*newSimulationElements.find("lbl_settingsThreadCountVal")).second->SetText(std::to_string(threadCount));
	(*newSimulationElements.find("lbl_settingsCarsPerThreadVal")).second->SetText(std::to_string(carsPerThread));
	(*newSimulationElements.find("lbl_settingsGenSizeVal")).second->SetText(std::to_string(generationSize));
	(*newSimulationElements.find("lbl_settingsMutationRateVal")).second->SetText(TruncateFloat(mutationRate) + "%");
	(*newSimulationElements.find("lbl_settingsMinMutationRateVal")).second->SetText(TruncateFloat(mutationMinRate) + "%");
	(*newSimulationElements.find("lbl_hlActivationVal")).second->SetText(activationFuncs[hiddenFuncID]);
	(*newSimulationElements.find("lbl_olActivationVal")).second->SetText(activationFuncs[outputFuncID]);

	//hidden layer data
	for (unsigned int i = 0; i < maxHiddenLayers; ++i) {
		std::string layerVal = (*newSimulationElements.find("txt_layerCount_" + std::to_string(i))).second->GetText();
		if (layerVal != "") hiddenLayerData[i] = std::min((unsigned)std::stoi(layerVal), maxNodes);
		(*newSimulationElements.find("lbl_hiddenLayerVal_" + std::to_string(i))).second->SetText(std::to_string(hiddenLayerData[i]));
		if (i >= hiddenLayers) {
			(*newSimulationElements.find("lbl_hiddenLayer_" + std::to_string(i))).second->Hide();
			(*newSimulationElements.find("lbl_hiddenLayerVal_" + std::to_string(i))).second->Hide();
		}
		else {
			(*newSimulationElements.find("lbl_hiddenLayer_" + std::to_string(i))).second->Show();
			(*newSimulationElements.find("lbl_hiddenLayerVal_" + std::to_string(i))).second->Show();
		}
	}
}

//draw========================
void MainMenu::Draw(sf::RenderTarget& window) {
	if (!hidden) {
		window.draw(background);
		window.draw(title);

		switch (currentState) {
		case(MenuState::Navigation):
			for (auto& m : navigationElements)
				m.second->Draw(window);
			break;
		case(MenuState::NewSimulation):
			for (const auto& m : newSimulationElements)
				m.second->Draw(window);
			(*newSimulationElements.find("dd_outputActivation")).second->Draw(window);
			(*newSimulationElements.find("dd_hiddenActivation")).second->Draw(window);
			break;
		}
	}
}

void MainMenu::Hide() {
	hidden = true;
	for (auto& m : navigationElements)
		m.second->Hide();
	for (auto& m : newSimulationElements)
		m.second->Hide();
}

void MainMenu::Show(bool cont) {
	//load continue button if applicable + reset
	withContinue = cont;
	navigationElements.clear();
	newSimulationElements.clear();	
	InitializeNavigation();
	InitializeNewSimulation();
	currentState = MenuState::Navigation;
	title.setString("Car Controller Neural Net Trainer");

	hidden = false;
	for (auto& m : navigationElements)
		m.second->Show();
	for (auto& m : newSimulationElements)
		m.second->Show();

	UpdateHLElements();
}