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
    ofParameter<float> animduration;
    ofParameter<int> FPS;
    ofParameter<void> encodeMp4Button;
    ofParameter<void> encodeGifButton;
    ofParameterGroup renderParameterGroup;

    PNGRenderer(float animduration, int fps, glm::vec2 resolution);
    std::string filePath;
    bool isCapturing = false;

    float Tick();
    void Start();
    void WritePNG(ofFbo *buffer);
    void AddToGui(ofxGuiPanel *panel);
    void UpdateResolution(int w, int h);
    ofParameter<bool> preview;
    ofParameter<void> saveButton;
    void saveGif();
    void saveVideo();
    int totalFrames;

private:
    int currentFrame;
    int renderedFrames;

};
