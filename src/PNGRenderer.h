#pragma once

#include <string>
#include "ofMain.h"
#include "ofxGuiExtended2.h"
#include "FFTManager.h"

class PNGRenderer {

public:
    string renderDirectory = "renders/";
    ofParameter<string> statusLabel;

    ofParameter<float> resolutionX;
    ofParameter<float> resolutionY;

    ofParameterGroup fileGroup;
    ofParameter<string> presetDisplayName;
    ofParameter<string> presetDisplayNameLabel;

    ofParameter<void> saveAsPresetButton;
    ofParameter<void> savePresetButton;
    ofParameter<void> openFileButton;
    ofParameter<void> mapMidiButton;
    ofParameter<void> webExportButton;

    ofParameter<float> displayScaleParam;
    ofParameter<int> frameskip;
    ofParameter<int> numLoops;
    ofParameter<int> numBlendFrames;
    ofParameter<float> animduration;
    ofParameter<int> FPS;
    ofParameter<void> encodeMp4Button;
    ofParameter<void> encodeGifButton;

    ofParameterGroup renderParameterGroup;
    ofParameterGroup pngSavingGroup;
    ofParameterGroup gifMenuGroup;
    ofParameterGroup vidMenuGroup;
    ofParameter<int> gifNumColors;

    PNGRenderer(float animduration, int fps, glm::vec2 resolution);
    string presetFilePath;

    bool isCapturing = false;

    float Tick();
    void Start();
    void WritePNG(ofFbo *buffer);
    void AddToGui(ofxGuiContainer *panel, FFTManager *fft);
    void UpdateResolution(int w, int h);
    void updatePath(string s);
    ofParameter<bool> preview;
    ofParameter<void> saveButton;
    int totalFrames;
    int currentFrame;

private:
    int renderedFrames;
    ofxGuiButton *saveFramesButton;
    void saveButtonPressed();
};
