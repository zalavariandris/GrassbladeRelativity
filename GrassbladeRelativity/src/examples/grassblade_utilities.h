#pragma once
#include <random>
#include <future>
#include <chrono> //sleep
#include <thread> //sleep
#include <functional>
#include "../Geo/Polygon.h"

using json = nlohmann::json;

struct Rect {
	glm::vec2 position;
	double width;
	double height;

	Rect(glm::vec2 position, double width, double height) :position(position), width(width), height(height) {};

	static glm::vec2 map(glm::vec2 P, Rect const & source, Rect const & target) {
		P.x = (P.x - source.position.x) / source.width;
		P.y = (P.y - source.position.y) / source.height;

		P.x = P.x*target.width + target.position.x;
		P.y = P.y*target.height + target.position.y;
		return P;
	}
};


double random() {
	static std::default_random_engine generator;
	static std::uniform_real_distribution<double> distribution(0, 1);
	static auto dice = std::bind(distribution, generator);
	return dice();
}

bool DragVec2(const char * label, glm::vec2 * P) {
	float v[2]{ P->x, P->y };
	if (ImGui::DragFloat2(label, v)) {
		P->x = v[0]; P->y = v[1];
		return true;
	}
	return false;
}

void addField(std::vector<glm::vec2> points, std::function<glm::vec2(glm::vec2)> field) {
	for (auto P : points) {
		addArrow(P, field(P), ImColor(255, 50, 255, 180));
	}
}

//glm::vec2 lerp(glm::vec2 a, glm::vec2 b, glm::vec2 t) {
//	return a + t * (b - a);
//};

/* Serialization helpers*/
namespace deserialize {
	std::vector<glm::vec2> points(json const & j)
	{
		std::vector<glm::vec2> points(j.size());
		for (auto i = 0; i < j.size(); i++) {
			double x = j[i][0].get<double>();
			double y = j[i][1].get<double>();
			points[i] = glm::vec2(x, y);
		}
		return points;
	}
}

namespace serialize {
	json points(std::vector<glm::vec2> points) {
		json j = {};
		cout << "!!!! " << j << endl;
		for (auto i = 0; i < points.size(); i++) {
			double x = points[i].x;
			double y = points[i].y;
			j.push_back({ x, y });
		}
		return j;
	}
}

namespace Animation {
	void to_json(json& j, const Keyframe& key) {
		j = { key.time() ,  key.value() };
	};

	void from_json(const json& j, Keyframe& key) {
		key.time(j.at(0).get<double>());
		key.value(j.at(1).get<double>());
	}
}

void fill(ofTexture * texture, std::function<glm::vec4(int x, int y)> f) {
	// fill to one-step map texture
	int w = texture->getWidth();
	int h = texture->getHeight();
	int c = 4;
	auto data = new float[w * h * c];
	for (int i = 0; i < w * h; i++) {
		int x = i % w;
		int y = i / w;

		glm::vec4 color = f(x, y);

		// fill data
		for (auto j = 0; j < c; j++) {
			data[i * c + j] = color[j];
		}
	}

	// upload data to GPU
	texture->loadData(
		data,
		w, h,
		GL_RGBA
	);
	delete[] data;
}

void distort(const ofTexture & image, const ofTexture & map, ofFbo * target) {
	static ofShader shader;
	if (!shader.isLoaded()) {
		cout << "load distort shader... ";
		shader.load("shadersGL3/distortShader");
		cout << (shader.isLoaded() ? "done" : "failed") << endl;
	}

	target->begin();
	ofClear(0, 0, 0, 0);
	ofSetColor(ofColor::white);
	shader.begin();
	shader.setUniformTexture("distortionMap", map, 1);
	image.draw(0, 0, target->getWidth(), target->getHeight());
	shader.end();
	target->end();
}
