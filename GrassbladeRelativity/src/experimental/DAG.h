#pragma once

#include <string>
#include <map>
#include "../Paper/Path.h"

class Node {
protected:
	std::string name;
	std::vector<std::string> pins;
	std::map<std::string, Node*> inputs;
public:
	Node() {};
	Node(std::string name) :name(name) { init(); }
	Node(std::string name, std::map<std::string, Node*> inputs) :name(name), inputs(inputs) { init(); };
	virtual void init() {};
	virtual void * get() {};
	virtual void evaluate() {};
};

class CreatePath : Node {
private:
	Paper::Path path;
public:
	using Node::Node;
	void init() {
		inputs["points"] = nullptr;
	}

	void evaluate() {

	}

	void * get() {
		return &path;
	}
};

class ExtendPath : Node {
public:
	using Node::Node;
	void init() {};
	void evaluate() {};
	void * get() {};
};

void exampleUsage() {
	Node sourcePath("sourcePath");
	Node targetPath("targetPath");
	ExtendPath("extendedPath", {"path", &sourcePath});
}