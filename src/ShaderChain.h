#pragma once
#include <vector>

#include "ofxGuiExtended2.h"
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
    void AudioTextureUpdate(ofTexture *audioTexture);
    void UpdateFft();
    void dragEvent(ofDragInfo info);

private:
    PNGRenderer *pngRenderer;
    ofxGui gui;
    ofxGuiGroup2 *parametersGuiGroup;
    ofxGuiPanel *guiGlobal;
    PassesGui *passesGui;
    ofxGuiPanel *parameterPanel;

    float mouseMoveSpeed = 10.0;
    FFTManager fft;

    bool showGui;
    bool isShowingFileDialogue;

    int frame;

    void RenderPasses();
    void removed(RemovedElementData& data);
    void moved(MovingElementData& data);
    void UpdateResolutionIfChanged();
    void OpenFilePressed();
    void LoadPassFromFile(string path);
};
