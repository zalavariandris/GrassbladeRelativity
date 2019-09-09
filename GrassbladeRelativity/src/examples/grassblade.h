#pragma once
#include "glm/glm.hpp"
#include <string>
#include <vector>
#include "../Animation/AnimCurve.h"
#include "of3dPrimitives.h"

class Grassblade {
public:
	Grassblade();
	~Grassblade();

	void pasteKeyframesFromAE();
	void open();
	void save();
	void draw();
private:

	std::string filepath;
	std::vector<glm::vec2> P1;
	Animation::AnimCurve Ax;
	Animation::AnimCurve Ay;
	Animation::AnimCurve Bx;
	Animation::AnimCurve By;
	Animation::AnimCurve Cx;
	Animation::AnimCurve Cy;
	std::string moviefile;
	ofPlanePrimitive plate;
	ofMesh plateMesh;
};

void showGrassblade();
