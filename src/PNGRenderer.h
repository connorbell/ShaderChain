#pragma once

#include <string>
#include "ofMain.h"
#include "ofxGui.h"

class PNGRenderer {

public:
    int resolutionX;
    int resolutionY;
    ofParameter<string> resolutionXParam;
    ofParameter<string> resolutionYParam;
    ofxButton savePresetButton;
    ofParameter<string> presetNameParam;
    ofParameter<float> displayScaleParam;
    ofParameter<int> frameskip;

    PNGRenderer(float duration, int fps, glm::vec2 resolution);
    std::string filePath;
    bool isCapturing = false;
    float duration;
    int FPS;

    float Tick();
    void Start();
    void WritePNG(ofFbo *buffer);
    void AddToGui(ofxPanel *panel);
    void UpdateResolution(int w, int h);

    ofParameter<bool> preview;

private:
    int currentFrame;
    int totalFrames;
    int renderedFrames;

    ofxButton saveButton;
    ofParameter<string> durationParam;
    ofParameter<string> fpsParam;
};
