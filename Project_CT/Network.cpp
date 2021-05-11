#include "Network.h"

Network::Network(int inputLayer, std::vector<int> hiddenLayers, int outputLayer, sf::FloatRect d, int hlActivationID, int olActivationID) : dimensions(d) {
	assert(hiddenLayers.size() > 0); //will not work with 0 hidden layers

	//setup nodes
	nodes_i = lin::Matrix(inputLayer, 1);
	nodes_o = lin::Matrix(outputLayer, 1);
	for (unsigned int i = 0; i < hiddenLayers.size(); ++i) {
		nodes_hl.emplace_back(hiddenLayers[i], 1);
	}

	//setup weights
	weights_ih = lin::Matrix(hiddenLayers[0], inputLayer);
	weights_ho = lin::Matrix(outputLayer, hiddenLayers[hiddenLayers.size() - 1]);
	weights_ih.Randomise(-5, 5);
	weights_ho.Randomise(-5, 5);
	if (hiddenLayers.size() > 1) {
		for (unsigned int i = 0; i < hiddenLayers.size() - 1; ++i) {
			//create hidden layer weights
			weights_hl.emplace_back(hiddenLayers[i + 1], hiddenLayers[i]);
			weights_hl[i].Randomise(-5, 5);
		}
	}

	//setup biases
	for (unsigned int i = 0; i < hiddenLayers.size(); ++i) {
		biases_hl.emplace_back(hiddenLayers[i], 1);
		biases_hl[i].Randomise(-1, 1);
	}
	biases_o = lin::Matrix(outputLayer, 1);
	biases_o.Randomise(-1, 1);

	//fetch activation functions - hidden layer
	switch (hlActivationID) {
	case(0): hiddenActivation = &lin::act_sigmoid; break;
	case(1): hiddenActivation = &lin::act_leakyRelu; break;
	case(2): hiddenActivation = &lin::act_binary; break;
	case(3): hiddenActivation = &lin::act_tanh; break;
	default: hiddenActivation = &lin::act_sigmoid; break;
	}
	//output layer
	switch (olActivationID) {
	case(0): outputActivation = &lin::act_sigmoid; break;
	case(1): outputActivation = &lin::act_leakyRelu; break;
	case(2): outputActivation = &lin::act_binary; break;
	case(3): hiddenActivation = &lin::act_tanh; break;
	default: outputActivation = &lin::act_sigmoid; break;
	}

	//SFML
	SetupRendering();
}

void Network::SetWeights(std::vector<lin::Matrix> newWeights) {
	if (newWeights.size() == weights_hl.size() + 2) {
		weights_ih = newWeights[0];
		weights_ho = newWeights[1];
		for (unsigned int i = 2; i < newWeights.size(); ++i)
			weights_hl[i - 2] = newWeights[i];
	}
	else std::cout << "Incorrect number of new weights for network.\n";
}

void Network::SetBiases(std::vector<lin::Matrix> newBiases) {
	if (newBiases.size() == biases_hl.size() + 1) {
		biases_o = newBiases[0];
		for (unsigned int i = 1; i < newBiases.size(); ++i) {
			biases_hl[i - 1] = newBiases[i];
		}
	}
	else std::cout << "Incorrect number of new biases for network.\n";
}

std::vector<lin::Matrix> Network::GetWeights() {
	std::vector<lin::Matrix> ret;
	ret.push_back(weights_ih);
	ret.push_back(weights_ho);
	for (unsigned int i = 0; i < weights_hl.size(); ++i)
		ret.push_back(weights_hl[i]);

	return ret;
}

std::vector<lin::Matrix> Network::GetBiases() {
	std::vector<lin::Matrix> ret;
	ret.push_back(biases_o);
	for (unsigned int i = 0; i < biases_hl.size(); ++i)
		ret.push_back(biases_hl[i]);

	return ret;
}

std::vector<float> Network::FeedForward(std::vector<float> inputs) {
	//convert inputs to matrix to use in multiplication
	if (inputs.size() == nodes_i.GetRows()) {
		lin::Matrix inputMatrix = lin::ToMatrix(inputs);
		nodes_i = inputMatrix;

		//activate first layer of hidden layer
		nodes_hl[0] = lin::MultiplyMatrices(weights_ih, inputMatrix);
		nodes_hl[0].Add(biases_hl[0]);
		nodes_hl[0].Map(hiddenActivation);

		//activate rest of hidden layer nodes
		for (unsigned int i = 1; i < nodes_hl.size(); ++i) {
			nodes_hl[i] = lin::MultiplyMatrices(weights_hl[i - 1], nodes_hl[i - 1]);
			nodes_hl[i].Add(biases_hl[i]);
			nodes_hl[i].Map(outputActivation);
		}

		//activate output layer
		nodes_o = lin::MultiplyMatrices(weights_ho, nodes_hl[nodes_hl.size() - 1]);
		nodes_o.Add(biases_o);
		nodes_o.Map(lin::act_binary);

		//SFML
		UpdateRender();
	}
	else std::cout << "Could not feed forward network. Incorrect number of inputs.\n";
	return nodes_o.ToVector();
}

//RENDERING ============================

void Network::Draw(sf::RenderTarget& window) {

	for (auto& l : connectionLines)
		window.draw(l);

	for (auto& c : inputNodes)
		window.draw(c);

	for (unsigned int i = 0; i < hiddenNodes.size(); ++i) {
		for (auto& n : hiddenNodes[i])
			window.draw(n);
	}

	for (auto& o : outputNodes)
		window.draw(o);
}

void Network::SetupRendering() {
	sf::CircleShape circle;
	float radius = 7.5;
	circle.setRadius(radius);
	circle.setOrigin(radius, radius);
	circle.setFillColor(sf::Color::White);

	float xInterval = dimensions.width / (nodes_hl.size() + 3);
	float yInterval = radius * 3;

	float padding;

	//hidden nodes
	int nodeCount = 0;
	for (unsigned int j = 0; j < nodes_hl.size(); ++j) {
		nodeCount += nodes_hl[j].GetRows();
		hiddenNodes.push_back(std::vector<sf::CircleShape>());

		padding = (dimensions.height - ((yInterval * nodes_hl[j].GetRows()) - yInterval)) / 2;
		for (unsigned int i = 0; i < nodes_hl[j].GetRows(); ++i) {
			circle.setPosition(sf::Vector2f(dimensions.left + xInterval * (j + 2), dimensions.top + padding + (i * yInterval)));
			hiddenNodes[j].push_back(circle);
		}
	}

	//hidden node weights
	if (hiddenNodes.size() > 1) {
		for (unsigned int j = 0; j < hiddenNodes.size() - 1; ++j) { //loop through layers
			for (unsigned int i = 0; i < hiddenNodes[j].size(); ++i) { //loop through nodes in layer
				for (unsigned int r = 0; r < hiddenNodes[j + 1].size(); ++r) { //loop through nodes in next layer
					sf::VertexArray connection = CreateLine(hiddenNodes[j][i].getPosition(), hiddenNodes[j + 1][r].getPosition(), weights_hl[j][r][i]);
					connectionLines.push_back(connection);
				}
			}
		}
	}

	//output nodes
	padding = (dimensions.height - ((yInterval * nodes_o.GetRows()) - yInterval)) / 2;
	for (unsigned int i = 0; i < nodes_o.GetRows(); ++i) {
		circle.setPosition(sf::Vector2f(dimensions.left + xInterval * (2 + nodes_hl.size()), dimensions.top + padding + (i * yInterval)));
		outputNodes.push_back(circle);

		//weights
		for (unsigned int j = 0; j < hiddenNodes[hiddenNodes.size() - 1].size(); ++j) {
			sf::VertexArray connection = CreateLine(hiddenNodes[hiddenNodes.size() - 1][j].getPosition(), circle.getPosition(), weights_ho[i][j]);
			connectionLines.push_back(connection);
		}
	}

	//input nodes
	padding = (dimensions.height - ((yInterval * nodes_i.GetRows()) - yInterval)) / 2;
	for (unsigned int i = 0; i < nodes_i.GetRows(); ++i) {
		circle.setPosition(sf::Vector2f(dimensions.left + xInterval, dimensions.top + padding + (i * yInterval)));
		inputNodes.push_back(circle);

		//weights
		for (unsigned int r = 0; r < weights_ih.GetRows(); ++r) {
			sf::VertexArray connection = CreateLine(circle.getPosition(), hiddenNodes[0][r].getPosition(), weights_ih[r][i]);
			connectionLines.push_back(connection);
		}
	}
}

//update colour of nodes
void Network::UpdateRender() {
	//input nodes
	for (unsigned int i = 0; i < inputNodes.size(); ++i) {
		unsigned int cv = std::min((int)(nodes_i[i][0] * 255.f), 255);
		inputNodes[i].setFillColor(sf::Color(cv, cv, cv));
	}

	//hidden layers 
	for (unsigned int i = 0; i < hiddenNodes.size(); ++i) {
		for (unsigned int j = 0; j < hiddenNodes[i].size(); ++j) {
			int cv = std::min((int)(nodes_hl[i][j][0] * 255.f), 255);
			hiddenNodes[i][j].setFillColor(sf::Color(cv, cv, cv));
		}
	}

	//output layer
	for (unsigned int i = 0; i < outputNodes.size(); ++i) {
		unsigned int cv = std::min((int)(nodes_o[i][0] * 255), 255);
		outputNodes[i].setFillColor(sf::Color(cv, cv, cv));
	}
}

//create weighted Line
sf::VertexArray Network::CreateLine(sf::Vector2f p1, sf::Vector2f p2, float weight) {
	sf::Color lineColor = weight < 0 ? sf::Color::Red : sf::Color::Green;
	weight = weight / 2;
	sf::Vector2f line = p2 - p1;
	sf::Vector2f normal(-line.y, line.x);
	normal = lin::Normalise(normal);
	sf::VertexArray returnShape(sf::TriangleStrip, 4);
	returnShape[0].position = p1 - weight * normal;
	returnShape[1].position = p1 + weight * normal;
	returnShape[2].position = p2 - weight * normal;
	returnShape[3].position = p2 + weight * normal;

	for (unsigned int i = 0; i < returnShape.getVertexCount(); ++i)
		returnShape[i].color = lineColor;

	return returnShape;
}

//SAVING / LOADING ============================

void Network::SaveToFile(std::ofstream &file) {
	//network details
	file << nodes_i.GetRows() << " " << nodes_o.GetRows();
	for (unsigned int i = 0; i < nodes_hl.size(); ++i) {
		file << " " << nodes_hl[i].GetRows();
	}
	file << std::endl;

	//input weights
	file << weights_ih.GetRows() << " " << weights_ih.GetCols() << std::endl;
	for (unsigned int r = 0; r < weights_ih.GetRows(); ++r) {
		for (unsigned int c = 0; c < weights_ih.GetCols(); ++c) {
			file << weights_ih[r][c] << " ";
		}
		file << std::endl;
	}

	//ouput weights
	file << weights_ho.GetRows() << " " << weights_ho.GetCols() << std::endl;
	for (unsigned int r = 0; r < weights_ho.GetRows(); ++r) {
		for (unsigned int c = 0; c < weights_ho.GetCols(); ++c) {
			file << weights_ho[r][c] << " ";
		}
		file << std::endl;
	}

	//hidden weights
	for (unsigned int i = 0; i < weights_hl.size(); ++i) {
		file << weights_hl[i].GetRows() << " " << weights_hl[i].GetCols() << std::endl;
		for (unsigned int r = 0; r < weights_hl[i].GetRows(); ++r) {
			for (unsigned int c = 0; c < weights_hl[i].GetCols(); ++c) {
				file << weights_hl[i][r][c] << " ";
			}
			file << std::endl;
		}
	}

	//ouput biases
	file << biases_o.GetRows() << " " << biases_o.GetCols() << std::endl;
	for (unsigned int r = 0; r < biases_o.GetRows(); ++r) {
		for (unsigned int c = 0; c < biases_o.GetCols(); ++c) {
			file << biases_o[r][c] << " ";
		}
		file << std::endl;
	}

	//hidden biases
	for (unsigned int i = 0; i < biases_hl.size(); ++i) {
		file << biases_hl[i].GetRows() << " " << biases_hl[i].GetCols() << std::endl;
		for (unsigned int r = 0; r < biases_hl[i].GetRows(); ++r) {
			for (unsigned int c = 0; c < biases_hl[i].GetCols(); ++c) {
				file << biases_hl[i][r][c] << " ";
			}
			file << std::endl;
		}
	}
}

std::vector<std::string> SplitString(std::string s, std::string delimiter) {
	std::vector<std::string> res;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		res.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	res.push_back(s);
	return res;
}

void LoadMatrix(std::ifstream& file, lin::Matrix& m) {
	std::string line;
	std::getline(file, line);
	std::vector<std::string> splitLine = SplitString(line, " ");
	m = lin::Matrix(std::stoi(splitLine[0]), std::stoi(splitLine[1]));
	for (unsigned int r = 0; r < m.GetRows(); ++r) {
		splitLine.clear();
		std::getline(file, line);
		splitLine = SplitString(line, " ");
		for (unsigned int c = 0; c < m.GetCols(); ++c) {
			m[r][c] = std::stof(splitLine[c]);
		}
	}
}

void Network::LoadFromFile(std::ifstream& file) {
	
	std::string line;	

	//setup network size - fetch node counts
	std::getline(file, line);
	std::vector <std::string> splitLine = SplitString(line, " ");
	nodes_i = lin::Matrix(std::stoi(splitLine[0]), 1);    //input
	nodes_o = lin::Matrix(std::stoi(splitLine[1]), 1);	  //output
	for (unsigned int i = 2; i < splitLine.size(); ++i) {
		nodes_hl.emplace_back(std::stoi(splitLine[i]), 1);//hidden
	}
	splitLine.clear();

	//load weights
	LoadMatrix(file, weights_ih);
	LoadMatrix(file, weights_ho);
	if (nodes_hl.size() > 1) {
		for (unsigned int i = 0; i < nodes_hl.size() - 1; ++i) {
			lin::Matrix hiddenWeight;
			LoadMatrix(file, hiddenWeight);
			weights_hl.push_back(hiddenWeight);
		}
	}

	//load biases
	LoadMatrix(file, biases_o);
	for (unsigned int i = 0; i < nodes_hl.size(); ++i) {
		lin::Matrix hiddenBias;
		LoadMatrix(file, hiddenBias);
		biases_hl.push_back(hiddenBias);
	}

	//SFML
	SetupRendering();
}

