#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "GrassbladeRelativity.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		ofxImGui::Gui gui;
		GrassbladeRelativity grassbladeRelativity;
};
