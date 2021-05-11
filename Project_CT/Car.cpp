#include "Car.h"

//helpers
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}
template<typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	assert(!(hi < lo));
	return (v < lo) ? lo : (hi < v) ? hi : v;
}

Car::Car(int id, sf::Vector2f pos, ResourceManager *resource, Track* trk)
	:ID(id), track(trk){

	position.x = pos.x / scale;
	position.y = pos.y / scale;

	//Fetch resources
	carBody.setTexture(*resource->GetCarTexture());
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> perc(0, 8);
	sf::IntRect carLoc = sf::IntRect(0, 24 * (perc(gen)), 40, 24);
	carBody.setTextureRect(carLoc);

	driftTexture = resource->GetDriftTexture();
	infoFont = resource->GetPixelFont();
	carBody.setScale(sf::Vector2f((cgToRear + cgToFront) / carBody.getTextureRect().width, (halfWidth * 2.f) / carBody.getTextureRect().height));

	//Track
	trackShapes = track->GetTrackShapes();
	tileSize = track->GetTileWidth();
	
	//CheckPointTracker tracker;
	checkPointTracker = CheckPointTracker(*track->GetCheckPoints());
	nextCheckpointBounds = checkPointTracker.GetNextCheckpointBounds();		

	//Info text
	for (unsigned int i = 0; i <= lineCount; ++i) {
		sf::Text text;
		text.setFont(*infoFont);
		text.scale(0.5f, 0.5f);
		text.setFillColor(sf::Color::Blue);
		infoText.push_back(text);
	}
	infoText[infoText.size() - 1].setString("ID: " + std::to_string(ID));

	//wheels
	for (int i = 0; i < 4; ++i) {
		sf::RectangleShape wheel;
		wheel.setSize(sf::Vector2f(wheelWidth * 4, wheelWidth * 2));
		wheel.setFillColor(sf::Color::Black);
		wheel.setOrigin(sf::Vector2f(wheelWidth * 2, wheelWidth));
		wheels.push_back(wheel);
	}		

	//distance lines to track edges
	distances = std::vector<float>(lineCount);

	scanArea = sf::RectangleShape(sf::Vector2f(track->GetTileWidth() * 2.f, track->GetTileWidth() * 2.f));
	scanArea.setOrigin(sf::Vector2f((float)track->GetTileWidth(), (float)track->GetTileWidth()));
	scanArea.setPosition(position);
	scanArea.setFillColor(sf::Color(255, 0, 0, 100));

	//global bounds
	globalBounds.setFillColor(sf::Color::Transparent);
	globalBounds.setOutlineColor(sf::Color::Blue);
	globalBounds.setOutlineThickness(1);

	//bounding box (rotated)
	float padding = 5.f;
	sf::Vector2f boundingBox = sf::Vector2f(carBody.getGlobalBounds().width * scale + padding, carBody.getGlobalBounds().height * scale + padding);
	collisionBounds.setPosition(position);
	collisionBounds.setOutlineColor(sf::Color::Red);
	collisionBounds.setPointCount(4);
	collisionBounds.setPoint(0, sf::Vector2f(0, 0));
	collisionBounds.setPoint(1, sf::Vector2f(0, boundingBox.y));
	collisionBounds.setPoint(2, sf::Vector2f(boundingBox.x, boundingBox.y));
	collisionBounds.setPoint(3, sf::Vector2f(boundingBox.x, 0));
	collisionBounds.setOrigin(boundingBox.x / 2, boundingBox.y / 2);
	collisionBounds.setFillColor(sf::Color::Transparent);
	collisionBounds.setOutlineThickness(1.f);		
}

void Car::DoPhysics(float dt) {
	float sn = std::sin(heading);
	float cs = std::cos(heading);

	//get velocity in local car coords
	velocity_c.x = cs * velocity.x + sn * velocity.y;
	velocity_c.y = cs * velocity.y - sn * velocity.x;

	//Weight on axles based on centre of gravity and weight shift due to forward/reverse acceleration
	float axleWeightFront = mass * (axleWeightRatioFront * gravity - weightTransfer * accel_c.x * cgHeight / wheelBase);
	float axleWeightRear = mass * (axleWeightRatioRear * gravity + weightTransfer * accel_c.x * cgHeight / wheelBase);

	//resulting velocity of the wheels as result of the yaw rate of the car body
	//v = yawRate * r where r is the distance from axle to CG and yawRate (angular velocity) in rad/s
	float yawSpeedFront = cgToFrontAxle * yawRate;
	float yawSpeedRear = -cgToRearAxle * yawRate;

	//calculate slip angles for front and rear wheels
	float slipAngleFront = std::atan2(velocity_c.y + yawSpeedFront, std::abs(velocity_c.x)) - sgn(velocity_c.x) * steerAngle;
	float slipAngleRear = std::atan2(velocity_c.y + yawSpeedRear, std::abs(velocity_c.x));

	if (slipAngleRear > 0.5 || slipAngleRear < -0.5) {
		addSkidMarks();
	}

	float tireGripFront = tireGrip;
	float tireGripRear = tireGrip * (1.f - I_ebrake * (1.f - lockGrip)); // reduce rear grip when ebrake is on

	float frictionForceFront_cy = clamp(-cornerStiffnessFront * slipAngleFront, -tireGripFront, tireGripFront) * axleWeightFront;
	float frictionForceRear_cy = clamp(-cornerStiffnessRear * slipAngleRear, -tireGripRear, tireGripRear) * axleWeightRear;

	//get amount of brake/throttle from inputs
	float brake(0), throttle(0);
	brake = std::min(I_brake * brakeForce + I_ebrake * eBrakeForce, brakeForce);
	throttle = I_throttle * engineForce;

	//resulting force in local car coordinates
	float tractionForce_cx = throttle - brake * sgn(velocity_c.x);
	float tractionForce_cy = 0;

	float dragForce_cx = -rollResist * velocity_c.x - airResist * velocity_c.x * std::abs(velocity_c.x);
	float dragForce_cy = -rollResist * velocity_c.y - airResist * velocity_c.y * std::abs(velocity_c.y);

	//total force in car coords
	float totalForce_cx = dragForce_cx + tractionForce_cx;
	float totalForce_cy = dragForce_cy + tractionForce_cy + std::cos(steerAngle) * frictionForceFront_cy + frictionForceRear_cy;

	//acceleration along car axes
	accel_c.x = totalForce_cx / mass;
	accel_c.y = totalForce_cy / mass;

	//acceleration in world coords
	accel.x = cs * accel_c.x - sn * accel_c.y;
	accel.y = sn * accel_c.x + cs * accel_c.y;

	//update velocity
	velocity.x += accel.x * dt;
	velocity.y += accel.y * dt;

	absVel = std::hypot(velocity.x, velocity.y); //length of vector?

	//calculate rotational forces 
	float angularTorque = (frictionForceFront_cy + tractionForce_cy) * cgToFrontAxle - frictionForceRear_cy * cgToRearAxle;

	//unstable at slow speeds just stop car
	if (std::abs(absVel) < 0.5 && !throttle) {
		velocity.x = velocity.y = absVel = 0;
		angularTorque = yawRate = 0;
	}

	float angularAccel = angularTorque / inertia;

	yawRate += angularAccel * dt;
	heading += yawRate * dt;

	//update position
	position.x += velocity.x * dt;
	position.y += velocity.y * dt;
}

float Car::ApplySmoothSteer(float steerInput, float dt) {
	float steer_l = 0;

	if (std::abs(steerInput) > 0.001f)
		steer_l = (float)clamp((steer + steerInput * dt * 2.f), -1.f, 1.f);
	else {
		if (steer > 0)
			steer_l = (float)std::max((steer - dt * 1.f), 0.f);
		else if (steer < 0)
			steer_l = (float)std::min(steer + dt * 1.f, 0.f);
	}
	
	return steer_l;
}

float Car::ApplySafeSteer(float steerInput) {	
	float avel = (float)std::min(absVel, 90.f);
	float steer_l = steerInput * (1.f - (avel / 4000.f));
	return steer_l;
}

void Car::Update(float dt) {
	if (alive) {
		scanArea.setPosition(position.x * scale, position.y * scale);
		onTrack = IsOnTrack();

		//alter traction on mud
		if (!onTrack) {
			rollResist = 600;
			tireGrip = 2.2f;
			alive = false;
		}
		else {
			rollResist = 8.f;
			tireGrip = 4.f;
		}

		float steerInput = I_rightSteer - I_leftSteer;

		if (smoothSteer)
			steer = ApplySmoothSteer(steerInput, dt);
		else steer = steerInput;

		if (safeSteer)
			steer = ApplySafeSteer(steer);

		steerAngle = steer * maxSteer;

		DoPhysics(dt);
		CalculateDistances();
		infoText[infoText.size() - 1].setPosition(collisionBounds.getTransform().transformPoint(collisionBounds.getPoint(3)));
		CheckPointHandling();
		fitness = CalculateFitness();
	}	
}

void Car::Draw(sf::RenderTarget& window, bool devOverlay){
	
	//dev mode below car
	if (selected) {
		if(devOverlay){
			//distance lines
			for (auto& l : distanceLines)
				window.draw(l);
		}
	}

	//temporarily removed due to performance - add option to add back / find way of fixing performance (NOT IMPORTANT)
	//skid marks
	//sf::Transform skidTransform;
	//skidTransform.scale(scale, scale);
	//for (sf::Sprite& c : skidMarks) 
	//	window.draw(c, skidTransform);

	sf::Transform transform;	
	transform.translate((position.x * (int)scale), (position.y * (int)scale));
	transform.scale(scale, scale);
	transform.rotate(heading * (180.f / (float)M_PI));
	//front wheels
	wheels.at(0).setPosition(cgToFrontAxle, halfWidth);
	wheels.at(1).setPosition(cgToFrontAxle, -halfWidth);
	wheels.at(0).setRotation((steerAngle * (180.f / (float)M_PI)));
	wheels.at(1).setRotation((steerAngle * (180.f / (float)M_PI)));

	//back wheels
	wheels.at(2).setPosition(-cgToRearAxle, halfWidth);
	wheels.at(3).setPosition(-cgToRearAxle, -halfWidth);
	wheels.at(2).setRotation(0);
	wheels.at(3).setRotation(0);

	//draw wheels
	window.draw(wheels.at(0), transform); 
	window.draw(wheels.at(1), transform);
	window.draw(wheels.at(2), transform);
	window.draw(wheels.at(3), transform);

	//car body
	sf::Transform transform2;	
	transform2.translate(position.x * scale, position.y * scale);
	transform2.scale(scale, scale);

	carBody.setPosition(0,0);			
	carBody.setOrigin(carBody.getTextureRect().width / 2.f, carBody.getTextureRect().height / 2.f);
	carBody.setRotation(heading * (180.f / (float)M_PI));

	collisionBounds.setPosition((position.x * (int)scale), (position.y * (int)scale));
	collisionBounds.setRotation(heading * (180.f / (float)M_PI));

	sf::FloatRect glbl = collisionBounds.getGlobalBounds();
	globalBounds.setPosition(glbl.left+5, glbl.top+5);
	globalBounds.setSize(sf::Vector2f(glbl.width-10, glbl.height-10));

	window.draw(carBody, transform2);		

	//dev display (above car)
	if (devOverlay) {
		//line distances
		if (selected) {
			for (auto& t : infoText) {
				window.draw(t);
			}
			window.draw(globalBounds);
		}
		window.draw(collisionBounds);
	}
}

void Car::addSkidMarks() {
	//temp skid func
	sf::Sprite temp1;	
	float cs = cos(heading);
	float sn = sin(heading);
	float posX = (position.x) - (cs * cgToRearAxle) - (sn * halfWidth);
	float posY = (position.y) - (sn * cgToRearAxle) + (cs * halfWidth);

	//right
	temp1.setScale(carBody.getScale());
	temp1.setTexture(*driftTexture);
	temp1.setOrigin(sf::Vector2f(temp1.getTextureRect().width / 2.f, temp1.getTextureRect().height / 2.f));
	temp1.setPosition(posX, posY);
	temp1.setRotation(heading * (180.f / (float)M_PI));
	skidMarks.push_back(temp1);	

	//left	
    posX = (position.x) - (cs * cgToRearAxle) + (sn * halfWidth);
	posY = (position.y) - (sn * cgToRearAxle) - (cs * halfWidth);
	temp1.setPosition(posX, posY);
	skidMarks.push_back(temp1);	
}

//calculate distances to the edges of the track, the car's 'vision'
void Car::CalculateDistances() {
	distanceLines.clear();
	distances.clear();

	//for each line
	for (unsigned int i = 0; i < lineCount; ++i) {
		sf::VertexArray newLine = sf::VertexArray(sf::LinesStrip);
		newLine.append(sf::Vertex(sf::Vector2f(position.x * scale, position.y * scale), lineColor));

		//create line of set length from center of car outwards in equal segments
		const float theta = (float)M_PI / (lineCount - 1);
		const float angle = (theta * i) - ((float)M_PI / 2.f);
		lin::Line line1 = lin::Line(sf::Vector2f(position.x * scale, position.y * scale), sf::Vector2f((position.x * scale) + (lineLength * cos(angle + heading)),
												 (position.y * scale) + (lineLength * sin(angle + heading))));

		float shortestDistance = (float)lineLength;
		for (auto& trackShape : *trackShapes) {	
			//only run check if the trackshape is within checking area
			if (trackShape.getGlobalBounds().intersects(scanArea.getGlobalBounds())) {								
				for (unsigned int i = 0; i < trackShape.getPointCount(); ++i) {
					//create line between two points of track shape
					lin::Line line2;
					if (i == trackShape.getPointCount() - 1) line2 = lin::Line(trackShape.getPoint(i), trackShape.getPoint(0));
					else line2 = lin::Line(trackShape.getPoint(i), trackShape.getPoint(i + 1));

					//get line2 length and check against track piece intersection 
					float Line2Length = line2.GetLength();
					float trackWidth = ((float)tileSize / 8) * 4;
					if (Line2Length < tileSize * 0.2 || Line2Length > tileSize * 0.9) {
						
						//check where lines intersect (not segments but full line)
						float delta = (line1.A * line2.B) - (line2.A * line1.B);
						if (delta != 0) { //if not parallel
							float x = (line2.B * line1.C - line1.B * line2.C) / delta;
							float y = (line1.A * line2.C - line2.A * line1.C) / delta;
							
							//check if segments both exist at that point
							if ((std::min(line2.p1.x, line2.p2.x) <= x + 1) && (x - 1 <= std::max(line2.p1.x, line2.p2.x)) &&
								(std::min(line2.p1.y, line2.p2.y) <= y + 1) && (y - 1 <= std::max(line2.p1.y, line2.p2.y)) &&
								(std::min(line1.p1.x, line1.p2.x) <= x + 1) && (x - 1 <= std::max(line1.p1.x, line1.p2.x)) &&
								(std::min(line1.p1.y, line1.p2.y) <= y + 1) && (y - 1 <= std::max(line1.p1.y, line1.p2.y))) {
								
								//check whether this is the shortest distance found so far
								float length = std::sqrt(std::pow(line1.p1.x - x, 2.f) + std::pow(line1.p1.y - y, 2.f));
								if (shortestDistance > length) {
									line1.p2 = sf::Vector2f(x, y);
									shortestDistance = length;
								}
							}
						}
					}
				}
			}
		}
		
		//save		
		newLine.append(sf::Vertex(line1.p2, lineColor));
		distanceLines.push_back(newLine);

		distances.push_back(shortestDistance / lineLength);

		//update distance value on UI
		std::string dist = std::to_string(shortestDistance);
		float text_x = line1.p2.x;
		float text_y = line1.p2.y;
		infoText[i].setPosition(text_x, text_y);
		infoText[i].setString(dist.substr(0, dist.find_first_of('.') + 2));			
	}	
}

bool Car::containsPoint(sf::Vector2f P) {
	sf::Vector2f A = collisionBounds.getTransform().transformPoint(collisionBounds.getPoint(0));
	sf::Vector2f B = collisionBounds.getTransform().transformPoint(collisionBounds.getPoint(1));
	sf::Vector2f C = collisionBounds.getTransform().transformPoint(collisionBounds.getPoint(2));
	sf::Vector2f D = collisionBounds.getTransform().transformPoint(collisionBounds.getPoint(3));

	return lin::doesRectContainPoint(P, A, B, C, D);
}

void Car::CheckPointHandling() {
	if (passedFinish) passedFinish = false;
	if (globalBounds.getGlobalBounds().intersects(nextCheckpointBounds)) {
		float prevFastest = checkPointTracker.GetFastestOfSegment();
		checkPointTracker.CompleteSegment();
		nextCheckpointBounds = checkPointTracker.GetNextCheckpointBounds();
		if (checkPointTracker.GetFastestOfSegment() != prevFastest)
			passedFinish = true;
	}
}

bool Car::IsOnTrack() {
	bool onTrack = false;
	sf::Vector2f pos = sf::Vector2f(position.x * scale, position.y * scale);
	for (auto &s : *trackShapes) {
		if (s.getGlobalBounds().intersects(scanArea.getGlobalBounds())) {
			if (lin::doesConvexShapeContainPoint(pos, s)) {
				if (s.getFillColor() == sf::Color::Red) {//check against inner corners
					onTrack = false;
					break;
				}
				onTrack = true;
			}
		}
	}
	return onTrack;
}

float Car::CalculateFitness() {
	//uses the amount of checkpoints crossed + distance to next checkpoint to calculate fitness
	if (checkPointTracker.Started()) {
		float fitness = checkPointTracker.GetCompletedSegments() * 2.f;
		lin::Line lineToNextCP(sf::Vector2f(position.x * scale, position.y * scale), checkPointTracker.GetNextCheckpointCenter());
		float distanceToNextCP = lineToNextCP.GetLength();
		fitness += 10 / distanceToNextCP;
		return fitness;
	}
	else return 0;
}

void Car::operator=(Car c) {
	position = c.position;
	velocity = c.velocity;
	velocity_c = c.velocity_c;
	accel = c.accel;
	accel_c = c.accel_c;

	heading = c.heading;
	absVel = c.absVel;
	yawRate = c.yawRate;
	steer = c.steer;
	steerAngle = c.steerAngle;

	driftTexture = c.driftTexture;
	carBody = c.carBody;
	wheels = c.wheels;
	skidMarks = c.skidMarks;

	track = c.track;
	tileSize = c.tileSize;

	distanceLines = c.distanceLines;
	trackShapes = c.trackShapes;
	scanArea = c.scanArea;
	distances = c.distances;

	infoText = c.infoText;
	infoFont = c.infoFont;

	collisionBounds = c.collisionBounds;
	globalBounds = c.globalBounds;

	selected = c.selected;
	ID = c.ID;

	checkPointTracker = c.checkPointTracker;
	nextCheckpointBounds = c.nextCheckpointBounds;

	onTrack = c.onTrack;

	alive = c.alive;
	fitness = c.fitness;
	timeAlive = c.timeAlive;
}