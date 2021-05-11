#pragma once

#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <math.h>
#include <algorithm>
#include <cassert>
#include <linearAlgebra.h>

#include "ResourceManager.h"
#include "CheckPointManager.h"
#include "CheckPointTracker.h"
#include "Track.h"

class Car
{ 
	//physics
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f velocity_c;
	sf::Vector2f accel;
	sf::Vector2f accel_c;

	float I_throttle = 0.f;
	float I_brake = 0.f;
	float I_ebrake = 0.f;
	float I_leftSteer = 0.f;
	float I_rightSteer = 0.f;
	
	float heading = 0.f;
	
	float absVel = 0.f;
	float yawRate = 0.f;
	float steer = 0.f;
	float steerAngle = 0.f;	
	
	//steering settings
	bool smoothSteer = true;
	bool safeSteer = true;

	//simulation settings 
	const float gravity = 9.81f;		//m/s
	const float mass = 1200.f;		//kg
	const float inertiaScale = 1.f;
	const float halfWidth = 1.2f;	//m
	const float cgToFront = 2.f;	//cg = center of gravity; 
	const float cgToRear = 2.f;
	const float cgToFrontAxle = 1.25f;
	const float cgToRearAxle = 1.25f;
	const float cgHeight = 0.55f;
	const float wheelRadius = 0.3f;
	const float wheelWidth = 0.2f;
	const float lockGrip = 0.7f;
	const float engineForce = 8000.0;
	const float brakeForce = 12000.0;
	const float eBrakeForce = brakeForce / 2.5f;
	const float weightTransfer = 0.2f;
	const float maxSteer = 0.4f;
	const float cornerStiffnessFront = 10.f;
	const float cornerStiffnessRear = 10.2f;
	const float airResist = 2.5f;	
	float tireGrip = 4.f;
	float rollResist = 8.f;

	float inertia = mass * inertiaScale;
	float wheelBase = cgToFrontAxle + cgToRearAxle;
	float axleWeightRatioFront = cgToRearAxle / wheelBase;
	float axleWeightRatioRear = cgToFrontAxle / wheelBase;

	//rendering car
	const float scale = 17.f;
	sf::Texture* driftTexture = nullptr;
	sf::Sprite carBody;
	std::vector<sf::RectangleShape> wheels;
	std::vector<sf::Sprite> skidMarks;	

	sf::ConvexShape collisionBounds;
	sf::RectangleShape globalBounds;

	//track info
	Track* track = nullptr;
	unsigned int tileSize = 0;
	std::vector<sf::ConvexShape>* trackShapes = nullptr;

	//Distances to track edge
	const unsigned int lineCount = 7;
	const unsigned int lineLength = 500;
	sf::RectangleShape scanArea; //area scanned for line intersections
	std::vector<float> distances;

	//secondary rendering
	sf::Color lineColor = sf::Color::Black;
	sf::Font* infoFont = nullptr;
	std::vector<sf::VertexArray> distanceLines;					
	std::vector<sf::Text> infoText;

	//checkpoint tracking
	CheckPointTracker checkPointTracker;
	sf::FloatRect nextCheckpointBounds;

	//member info
	int ID = -1;
	bool selected = false;
	bool onTrack = true;
	bool alive = true;
	float fitness = 0.f;
	sf::Clock timeAlive;

	//private functions			
	float ApplySmoothSteer(float steerInput, float dt);
	float ApplySafeSteer(float steerInput);
	float CalculateFitness();
	void DoPhysics(float dt);
	void addSkidMarks();
	void CalculateDistances();
	void CheckPointHandling();

	bool passedFinish = false;

public:	
	Car(){}
	Car(int id, sf::Vector2f pos, ResourceManager *resource, Track* track);
	
	void Update(float dt);
	void Draw(sf::RenderTarget& window, bool devOverlay);
	bool containsPoint(sf::Vector2f);	
	bool IsOnTrack();
	
	void operator=(Car c);

	//gets
	inline int getID() { return ID; }
	inline int GetLineCount() { return lineCount; }
	inline int GetMaxLineLength() { return lineLength; }
	inline bool IsAlive() { return alive; }
	inline bool HasStarted() { return checkPointTracker.Started(); }
	inline bool isSelected() { return selected; }
	inline float GetFitness() { return fitness; }	
	inline float GetRotation() { return heading * (180.f / (float)M_PI); }		

	inline std::vector<float> GetDistances() { return distances; }	
	inline sf::Vector2f getPosition() { return sf::Vector2f(position.x * scale, position.y * scale); }
	inline sf::RectangleShape getScanArea() { return scanArea; }
	inline sf::RectangleShape getGlobalBounds() { return globalBounds; }

	//toggle selection
	inline void Select() {
		selected = true;
		collisionBounds.setOutlineColor(sf::Color::Green);
	}
	inline void Deselect() {
		selected = false;
		collisionBounds.setOutlineColor(sf::Color::Red);
	}			

	inline std::vector<float> GetVision() {
		std::vector<float> nnInput = distances;
		nnInput.push_back(absVel / 30);
		return nnInput;
	}
	inline void SetInputs(std::vector<float> inputs) {
		if (inputs.size() > 0) {
			I_throttle = inputs[0];
			I_brake = inputs[1];
			I_ebrake = inputs[2];
			I_rightSteer = inputs[3];
			I_leftSteer = inputs[4];
		}
	}	

	//checkpoint management
	inline bool HasPassedFinish() { return passedFinish; }
	inline float GetLastLap() { return checkPointTracker.GetLastLapTime(); }
};

