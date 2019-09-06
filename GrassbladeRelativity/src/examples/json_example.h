#pragma once

#include <json.hpp>
//#include <iostream>
#include <fstream>
#include "glm/glm.hpp"
using json = nlohmann::json;

using nlohmann::json;

// read a JSON file
json read(std::string filename) {
	std::ifstream i(filename);
	nlohmann::json j;
	i >> j;
	return j;
}

// write prettified JSON to another file
void write(json j, std::string filename) {
	std::ofstream o(filename);
	o << std::setw(4) << j << std::endl;
	o.close();
}

namespace ns {
	// a simple struct to model a person
	struct person {
		std::string name;
		std::string address;
		int age;
	};
}

namespace ns {
	void to_json(json& j, const person& p) {
		j = json{ {"name", p.name}, {"address", p.address}, {"age", p.age} };
	}

	void from_json(const json& j, person& p) {
		//j.at("name").get_to(p.name);
		//j.at("address").get_to(p.address);
		//j.at("age").get_to(p.age);
		p.name = j.at("name").get<std::string>();
		p.address = j.at("address").get<std::string>();
		p.age = j.at("age").get<int>();
	}
} // namespace ns


//namespace glm {
//	void to_json(json& j, const glm::vec2& P) {
//		j = json::object({ {"x", P.x}, {"y", P.y} });
//	}
//
//	void from_json(const json& j, glm::vec2& P) {
//		P.x = j.at("x").get<double>();
//		P.y = j.at("y").get<double>();
//	}
//}

namespace Geo {
	struct Point {
		double x;
		double y;
		Point(double x, double y) : x(x), y(y) {};
	};

	void to_json(json& j, const Point& P) {
		j = { { "x", P.x }, { "y", P.y } };
	};

	void from_json(const json& j, Point& P) {
		P.x = j.at("x").get<double>();
		P.y = j.at("y").get<double>();
	}

}

namespace glm {
	void to_json(json& j, const glm::vec2& P) {
		j = { { "x", P.x }, { "y", P.y } };
	};

	void from_json(const json& j, glm::vec2& P) {
		P.x = j.at("x").get<double>();
		P.y = j.at("y").get<double>();
	}

}


void showJsonExample() {

	json j;
	std::vector<Geo::Point> points{ { 70, -140}, {30, -30}, {20, 125} };
	j["points"] = points;

	std::cout << j.dump() << std::endl;
}