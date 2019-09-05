#pragma once

#include <json.hpp>
//#include <iostream>
//#include <fstream>
// for convenience
using json = nlohmann::json;

namespace{
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
}

void showJsonExample() {
	json j = {
		{"file", "C:/Users/andris/Pictures/2019-08/IMG_6926.MOV"},
		{"points", { { 70, -140 }, { 30, -30 }, { 20, 125 } }}
	};

	//auto j3 = nlohmann::json::parse("");

	std::cout << j.dump() << std::endl;

	write(j, "bin/data/grassblade.json");
}