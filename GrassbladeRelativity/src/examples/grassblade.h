#pragma once
#include "../Project.h"

#include "glm/glm.hpp"
#include <string>
#include <vector>
#include "../Animation/AnimationCurve.h"
#include "of3dPrimitives.h"
#include <utility> //std::pair
#include "../Reader.h"
#include "../Paper/Path.h"


class Grassblade : Project {
public:
	Grassblade();
	~Grassblade();
	void tick();

	void importKeysFromAE();
	void importKeysFromMaya();
	void open();
	void save();
private:

	std::string filepath{ ofToDataPath("projects/grassblade.json") };
	
	// scene
	Reader reader;
	Paper::Path sourcePath;
	Paper::Path targetPath;
	Animation::AnimationCurve Ax;
	Animation::AnimationCurve Ay;

	// application
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
	bool autokey{ false };
	void ShowRender();
	void ShowPreview();

	bool weighted{ false };
};

