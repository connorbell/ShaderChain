#pragma once
#include <vector>

#include "ofxGui.h"
#include "ofxMidi.h"
#include "ofxJSON.h"

#include "ShaderPass.h"
#include "PNGRenderer.h"
#include "FFTManager.h"
#include "PassesGui.h"

class ShaderChain: public ofxMidiListener {
public:
    std::vector<ShaderPass*> passes;
    float time;
    bool isRunning;
    ofxMidiIn midiIn;
    ofxJSONElement result;
    ofNode camera;
    bool isMouseDown;

    ShaderChain(glm::vec2 res);
    ShaderChain() {}
    ~ShaderChain();
    void Setup(glm::vec2 res);
    void SetupGui();
    void BeginSaveFrames();
    void Update();
    void AddPass(ShaderPass *pass);

    void UpdateCamera();
    void KeyPressed(int key);
    void newMidiMessage(ofxMidiMessage& eventArgs);
    void WriteToJson();
    void ReadFromJson(std::string path);
    void SetupMidi();
    void ResolutionDidChange(string &val);
    void AudioTextureUpdate(ofTexture *audioTexture);
    void UpdateFft();

private:
    PNGRenderer *pngRenderer;
    ofxPanel gui;
    ofxPanel guiGlobal;
    PassesGui passesGui;

    float mouseMoveSpeed = 10.0;
    FFTManager fft;

    bool showGui;

    void RenderPasses();

};
