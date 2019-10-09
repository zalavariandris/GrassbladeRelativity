#pragma once

#include "ofMain.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp{
public:
	//imgui
	ofxImGui::Gui gui;
	float guiScale{ 2 };
	ImFont* font1;
	ImFont *font2;

	//of
	void setup();
	void draw();
	void update();

	
};
