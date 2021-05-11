#pragma once
#include <SFML/Graphics.hpp>
#include <linearAlgebra.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

std::vector<std::string> SplitString(std::string, std::string);

class Network {
	//nodes
	lin::Matrix nodes_i; //input nodes
	lin::Matrix nodes_o; //output nodes
	std::vector<lin::Matrix> nodes_hl; //hidden layer nodes

	//weights
	lin::Matrix weights_ih; //weights input - hidden
	lin::Matrix weights_ho; //weights output - hidden
	std::vector<lin::Matrix> weights_hl; //weights in hidden layer
	
	//biases
	lin::Matrix biases_o; //output biases
	std::vector<lin::Matrix> biases_hl; //hidden layer biases

	//Rendering
	sf::FloatRect dimensions;
	std::vector<sf::CircleShape> inputNodes;
	std::vector<std::vector<sf::CircleShape>> hiddenNodes;
	std::vector<sf::CircleShape> outputNodes;

	std::vector<sf::VertexArray> connectionLines;

	void SetupRendering();
	void UpdateRender();
	sf::VertexArray CreateLine(sf::Vector2f p1, sf::Vector2f p2, float weight);

	float (*hiddenActivation)(float) = nullptr;
	float (*outputActivation)(float) = nullptr;

public:
	Network() {};
	Network(int inputLayer, std::vector<int> hiddenLayers, int outputLayer, sf::FloatRect d, int hlActivationID, int olActivationID);

	void SetWeights(std::vector<lin::Matrix> newWeights);
	void SetBiases(std::vector<lin::Matrix> newBiases);
	std::vector<lin::Matrix> GetWeights();
	std::vector<lin::Matrix> GetBiases();

	std::vector<float> FeedForward(std::vector<float> inputs);			

	void SaveToFile(std::ofstream& file);
	void LoadFromFile(std::ifstream& file);

	void Draw(sf::RenderTarget& window);
};