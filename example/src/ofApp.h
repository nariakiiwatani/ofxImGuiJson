#pragma once
#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxImGuiJson.h"
#include <nlohmann/json.hpp>

class ofApp : public ofBaseApp {
public:
	void setup() override;
	void draw() override;

private:
	ofxImGui::Gui gui;
	nlohmann::json schema;
	nlohmann::json data;
};
