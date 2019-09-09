#include "ofApp.h"
#include "examples/grassblade.h"
//#include "examples/im2d_demos.h"
//#include "examples/json_example.h"

void ofApp::setup() {
	/* imgui */
	gui.setup();
	ImGui::GetIO().FontGlobalScale = 1.0;
	
	/* OF */
	ofDisableArbTex();
	//ofEnableNormalizedTexCoords(); // needless with disabled ARB textures, but left it here for clarity.
	ofSetVerticalSync(true);
	ofSetBackgroundColor(ofColor(10, 10, 10));
	//ofToggleFullscreen();
}

void ofApp::update() {
	
}

#include "Animation/AnimCurve.h"
/*
AE keyframe data example: 
```
Adobe After Effects 8.0 Keyframe Data

	Units Per Second	29.97
	Source Width	1920
	Source Height	1080
	Source Pixel Aspect Ratio	1
	Comp Pixel Aspect Ratio	1

Motion Trackers	Tracker #1	Track Point #1	Confidence
	Frame	percent
	0	100
	1	96.1274

Motion Trackers	Tracker #1	Track Point #1	Attach Point
	Frame	X pixels	Y pixels
	0	938.125	525.75
	1	937.273	526.008

Motion Trackers	Tracker #1	Track Point #1	Search Size
	Frame	X 	Y
		40	40

Motion Trackers	Tracker #1	Track Point #1	Feature Center
	Frame	X pixels	Y pixels
	0	938.125	525.75
	1	937.273	526.008

Motion Trackers	Tracker #1	Track Point #1	Attach Point Offset
	Frame	X 	Y
		0	0

Motion Trackers	Tracker #1	Track Point #1	Search Offset
	Frame	X 	Y
		0	0

Motion Trackers	Tracker #1	Track Point #1	Feature Size
	Frame	X 	Y
		20	20


End of Keyframe Data
```
*/


class AEKeyframes {
public:
	AEKeyframes(){
		std::vector<Animation::AnimCurve> curves;

		std::string filename{ ("projects/aeKeyframesData.txt") };
		std::string line;
		ifstream f(ofToDataPath(filename));
		if (!f.is_open())
			cout << "error while opening file" << endl;

		// Process title
		// skip to metadatablock block
		while (std::getline(f, line) && line != "") {}

		// skip to keyframes block
		while (std::getline(f, line) && line != "") {}

		/* Process keyframes data */
		// (process first block of keyframes only)
		// process block header (simply step through)
		std::getline(f, line);
		// Process csv heading (simply step throuh)
		std::getline(f, line);

		// process csv rows one by one
		Animation::AnimCurve curveX, curveY;
		while(std::getline(f, line) && line!=""){
			std::string cell;
			std::stringstream linestream(line);
			int frame;
			float x, y;
			linestream >> frame >> x >> y;
			curveX.setValueAtFrame(frame, x);
			curveY.setValueAtFrame(frame, y);
		}
		if (f.bad()) {
			cout << "error while reading file" << endl;
		}

		//
		for (auto i = 0; i < curveX.getKeys().size(); i++) {
			cout << curveX.getKeys().at(i).frame << ", " << curveX.getValueAtFrame(i) << ", " << curveY.getValueAtFrame(i) << endl;
		}
	}

	void show() {
		ImGui::Begin("AEKeyframes");
		//int i = 0;
		//int frame;
		//float x, y;
		//while (myfile >> frame) {
		//	myfile >> x >> y;
		//	frames.push_back(frame);
		//	positions.push_back(glm::vec2(x, y));
		//}
		//for (auto i = 0; i < frames.size(); i++) {
		//	ImGui::Text("%i [%g, %g]", frames[i], positions[i].x, positions[i].y);
		//}

		ImGui::End();
	}
};

void ofApp::draw() {
	gui.begin();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("StyleEditor"))
		{
			ImGui::ShowStyleEditor();
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	showGrassblade();
	//showJsonExample();

	//static AEKeyframes aeKeyframes;
	//aeKeyframes.show();

	gui.end();
}