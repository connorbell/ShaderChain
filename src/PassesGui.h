#pragma once

#include <vector>
#include "ofxGui.h"
#include "ShaderPass.h"

class PassesGui {
public:
    std::vector<ofxButton*> passButtons;
    ofxButton addPassButton;

    void Setup(std::vector<ShaderPass*> passes);
    void Draw();

private:
    ofxPanel gui;

};
