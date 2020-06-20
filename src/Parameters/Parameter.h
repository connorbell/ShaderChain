#pragma once
#include <string>

#include "ofMain.h"
#include "ofxGuiExtended2.h"
#include "ofxAutoReloadedShader.h"
#include "ofxJSON.h"
#include "TextureInputSelectionView.h"
#include "RenderStruct.h"

typedef enum {
    ImageFile,
    VideoFile,
    Buffer,
    Webcam,
    Last,
    Audio,
    None
} TextureSourceType;

class ShaderPass;

class Parameter {
public:

    ~Parameter();
    std::string uniform;
    bool show;
    TextureInputSelectionView *selectionView;
    string parentBufferName;
    string type = "";

    virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {}
    virtual void AddToGui(ofxGuiGroup2 *gui) {}
    virtual void UpdateJson(Json::Value &val) {}
    virtual void UpdateMidi(int midiIndex, float value) {}
    virtual void BindMidi(int midiIndices[]) {}
    virtual void bindMidi(int midiIndex, int subParamIndex) {}
    virtual void update(RenderStruct *renderStruct) {}
    virtual void startOfflineRender() {}
    virtual void stopOfflineRender() {}
    virtual bool isMouseHoveredOver() { return false; }
    virtual void handleInputFile(string s) {}
    virtual TextureSourceType getTextureSourceType() { return None; }
    virtual void playbackDidToggleState(bool isPaused) {}
    virtual void close() {};
    virtual Json::Value getDict() {};
};
