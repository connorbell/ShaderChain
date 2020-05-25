#pragma once

#include "Parameter.h"
#include "ofxGuiExtended2.h"
#include "ofxGuiTextureInput.h"
#include "ofGstVideoPlayer.h"

typedef enum {
    ImageFile,
    VideoFile,
    Buffer,
    Webcam,
    None
} TextureSourceType;

class TextureParameter : public Parameter {

public:
    TextureParameter(string uniform, string filePath, int textureIndex, bool show);
    ofImage value;
    string filePath;
    TextureSourceType type;
    int textureIndex;
    ofVideoPlayer videoFile;
    string bufferName;
    ofxGuiTextureInput *texInput;
    ofGstVideoPlayer gstreamer;
    void updateTextureFromFile(string &s);
    void wantsWebcamChanged(bool &val);
    void wantsBufferChanged(string &val);
    void onShowSelectionView();
    void onHideSelectionView();
    virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) override;
    virtual void AddToGui(ofxGuiGroup2 *gui) override;
    virtual void UpdateJson(Json::Value &val) override;
    virtual void update(RenderStruct *renderStruct) override;
    virtual void startOfflineRender() override;
    virtual void stopOfflineRender() override;
};
