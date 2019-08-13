#include "ofApp.h"
#include "Im2D/Im2D.h"
//--------------------------------------------------------------
void ofApp::setup(){
	gui.setup();
	grassbladeRelativity.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
	grassbladeRelativity.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	gui.begin();
	grassbladeRelativity.draw();
	gui.end();
}
