#pragma once
#include "SFML/Graphics.hpp"
#include "CheckPoint.h"
#include "CheckPointManager.h"

struct Segment {

	Segment(int id, CheckPoint& _c1, CheckPoint& _c2) {
		c1 = _c1;
		c2 = _c2;
		ID = id;
	}

	int ID;
	CheckPoint c1, c2;
	std::vector<float> timings;

	float getLast() { return timings[timings.size() - 1]; }
	float GetFastest() {
		return (std::min_element(timings.begin(), timings.end()) != timings.end()) ?
			*std::min_element(timings.begin(), timings.end()) : 0;
	}
};

class CheckPointTracker {
	std::vector<CheckPoint> checkpoints;
	std::vector<Segment> segments;
	std::vector<float> lapTimes;
	
	sf::Clock lapClock;
	sf::Clock segmentClock;
	
	unsigned int currentSegmentID = 0;
	unsigned int completedSegments = 0;
	
	bool hasStarted = false;

public:
	CheckPointTracker(){}
	CheckPointTracker(CheckPointManager& checkpointManager);
	void StartTracking();
	void CompleteSegment();

	inline float GetLastLapTime() { return lapTimes.size() > 0 ? lapTimes[lapTimes.size() - 1] : 0.f; }	
	inline float GetFastestOfSegment() { return segments[currentSegmentID].GetFastest(); }
	inline float GetCurrentSegmentTime() { return !hasStarted ? 0.f : segmentClock.getElapsedTime().asMilliseconds(); }
	
	inline int GetCompletedSegments() { return completedSegments; }
	inline bool Started() { return hasStarted; }

	inline sf::FloatRect  GetNextCheckpointBounds() {
		return !hasStarted ? segments[0].c1.getGlobalBounds() : segments[currentSegmentID].c2.getGlobalBounds();
	}
	inline sf::Vector2f GetNextCheckpointCenter() {
		return !hasStarted ? segments[0].c1.GetCenter() : segments[currentSegmentID].c2.GetCenter();
	}
};
