#pragma once
#include "glm/glm.hpp"
#include <string>
#include <vector>
#include "../Animation/AnimCurve.h"
#include "of3dPrimitives.h"
#include <utility> //std::pair
#include "../Reader.h"
#include "../Paper/Path.h"
class Grassblade {
public:
	Grassblade();
	~Grassblade();

	void importKeysFromAE();
	void importKeysFromMaya();
	void open();
	void save();
	void draw();
	void onGui();
	void render();
private:

	std::string filepath{ ofToDataPath("projects/grassblade.json") };
	
	Paper::Path sourcePath;
	Paper::Path targetPath;
	Animation::AnimCurve Ax;
	Animation::AnimCurve Ay;
	std::string moviefile;
	ofPlanePrimitive plate;
	ofMesh plateMesh;
	int startTime{0};
	int endTime{99};
	float extensionLength{ 100 };
	float deformFactor{ 1.0 };

	int F;
	bool play;

	ofTexture distortionTexture;
	ofFbo renderFbo;

	ofCamera cam;

	Reader reader;

	bool autokey{ false };
};

void showGrassblade();
