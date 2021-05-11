#pragma once
#include <iomanip>
#include <sstream>
#include <linearAlgebra.h>
#include <thread>
#include <algorithm>
#include <numeric>

#include "Car.h"
#include "Network.h"
#include "Track.h"
#include "ThreadPool.h"

//helpers (saving / display)
std::string FloatToTime(float n);
std::string TruncateFloat(float n);

struct TrainerData {
	TrainerData() {}
	unsigned int generationSize = 0;
	unsigned int currentGeneration = 0;
	float bestLapTime = 0.f; //ms
	float bestLapTimePrev = 0.f; //ms
	float bestFitness = 0.f;
	float bestFitnessPrev = 0.f;
	float currentTime = 0.f;
	float totalTime = 0.f;
	std::string hlActivation;
	std::string olActivation;
	float mutationRate = 0.f;
	float mutationRateMinor = 0.f;
};

class Trainer
{	
	std::vector<std::string> activationFuncs{ "Sigmoid", "Leaky RELU", "Binary Step", "Tanh" };
	unsigned int hiddenActivationID = 0;
	unsigned int outputActivationID = 0;

	//training settings
	int threadCount = 1;
	int carsPerThread = 50;
	int generationSize = 50;
	int currentGeneration = 1;
	bool running = false;

	float elapsedTime = 0.f;
	sf::Clock totalTime;
	sf::Clock generationTimer;
	const float maxGenTime = 40; //seconds

	const int surviverPool = 10;
	float mutationRate = 0.01f;
	float mutationMinRate = 0.05f;	
	
	//network settings
	const int inputNodes = 8;
	const int outputNodes = 5;
	std::vector<int> hiddenNodes = { 6, 5 };	
	sf::FloatRect nnDimensions;

	//entities
	Track track;
	std::vector<Car> cars;
	std::vector<Network> networks;
	int currentId = 0;

	std::vector<Network> bestNetworks;
	Network bestNetwork;

	//saved data
	std::vector<float> bestFitnessPerGen;	
	std::vector<float> avgFitnessPerGen;

	//data
	std::vector<float> currentLapTimes;
	float bestLapTime = 0.f;
	float bestLapTimePrev = 0.f;
	float bestFitness = 0.f;
	float bestFitnessPrev = 0.f;

	//private funcs
	void NewScene();
	void UpdateRange(float, int, int);
	void Mutate(lin::Matrix& m);	
	
	//quick sort
	int Partition(int low, int high);
	void SortCars(int low, int high);

	//managers
	ResourceManager* resourceManager;
	
public:
	Trainer(ResourceManager* rMngr, Track& track, sf::FloatRect nnDim);	
	void SetupTrainer(int threadCount, int carsPerThread, std::vector<int> hiddenLayers, int hlActivationID, int olActivationID, float mutationRate, float minMutationRate);

	void Update(float dt, ThreadPool &pool);
	void DrawEntities(sf::RenderTarget& window, bool devOverlay);
	void DrawUI(sf::RenderTarget& window);	
	void NextGeneration(bool skipReset);

	bool SaveScene(std::string filename);
	bool LoadScene(std::string filename);

	bool ExportData(std::string filename);

	static float Divide(float n);

	inline bool IsRunning() { return running; }
	inline std::vector<Car>& GetCars() { return cars; }
	inline int GetCurrentID() { return currentId; }
	inline void SetCurrentID(int newID) { currentId = newID; }
	inline void ResetScene() { if (generationSize > 0) NewScene(); else std::cout << "Could not reset scene."; }
	
	inline void Pause() { running = false; }
	inline void Continue() { running = true; }	

	//get data for UI
	inline TrainerData GetData() {
		TrainerData tData;
		tData.generationSize = generationSize;
		tData.currentGeneration = currentGeneration;
		tData.bestLapTime = bestLapTime;
		tData.bestLapTimePrev = bestLapTimePrev;
		tData.bestFitness = bestFitness;
		tData.bestFitnessPrev = bestFitnessPrev;
		tData.currentTime = generationTimer.getElapsedTime().asMilliseconds();
		tData.totalTime = elapsedTime + totalTime.getElapsedTime().asMilliseconds();
		tData.hlActivation = activationFuncs[hiddenActivationID];
		tData.olActivation = activationFuncs[outputActivationID];
		tData.mutationRate = mutationRate;
		tData.mutationRateMinor = mutationMinRate;
		return tData;
	}
};

