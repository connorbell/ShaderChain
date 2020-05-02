#pragma once

#include <string>
#include "ofMain.h"
#include "ofxGuiExtended2.h"

class PNGRenderer {

public:
    ofParameter<string> statusLabel;

    ofParameter<float> resolutionX;
    ofParameter<float> resolutionY;

    ofParameter<void> savePresetButton;
    ofParameter<void> openFileButton;

    ofParameter<string> presetNameParam;
    ofParameter<float> displayScaleParam;
    ofParameter<int> frameskip;
    ofParameter<float> duration;
    ofParameter<int> FPS;

    PNGRenderer(float duration, int fps, glm::vec2 resolution);
    std::string filePath;
    bool isCapturing = false;

    float Tick();
    void Start();
    void WritePNG(ofFbo *buffer);
    void AddToGui(ofxGuiPanel *panel);
    void UpdateResolution(int w, int h);
    ofParameter<bool> preview;
    ofParameter<void> saveButton;

private:
    int currentFrame;
    int totalFrames;
    int renderedFrames;

};
