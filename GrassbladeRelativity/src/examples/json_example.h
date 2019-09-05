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

void showJsonExample() {
	ns::person p = { "Ned Flanders", "744 Evergreen Terrace", 60 };
	std::vector<glm::vec2> points{ { 70, -140 }, { 30, -30 }, { 20, 125 } };
	json j;
	j["file"] = { "file", "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV" };
	j["person"] = p;

	//auto j3 = nlohmann::json::parse("");

	std::cout << j.dump() << std::endl;

	//write(j, "json_test.json");
}