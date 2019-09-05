#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp{
public:
	//imgui
	ofxImGui::Gui gui;

	//of
	void setup();
	void draw();
	void update();
};
