#pragma once

#include "Parameter.h"
#include "ofxGuiExtended2.h"
#include "ofxGuiTextureInput.h"
#include "ofGstVideoPlayer.h"

class TextureParameter : public Parameter {

public:
    TextureParameter(string uniform, string filePath, int textureIndex, bool show, string texType, string targetBufferName);
    ofImage value;
    string filePath;
    TextureSourceType type;
    int textureIndex;
    ofVideoPlayer videoFile;
    string targetBufferName;
    ofxGuiTextureInput *texInput;
    ofGstVideoPlayer gstreamer;
    void updateTextureFromFile(string &s);
    void wantsWebcamChanged(bool &val);
    void wantsBufferChanged(string &val);
    void onShowSelectionView();
    void onHideSelectionView();
    void updateToNewType(TextureSourceType t);

    virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) override;
    virtual void AddToGui(ofxGuiGroup2 *gui) override;
    virtual void UpdateJson(Json::Value &val) override;
    virtual void update(RenderStruct *renderStruct) override;
    virtual void startOfflineRender() override;
    virtual void stopOfflineRender() override;
    virtual bool isMouseHoveredOver() override;
    virtual void handleInputFile(string s) override;
    virtual TextureSourceType getTextureSourceType() override;
    virtual void playbackDidToggleState(bool isPaused) override;

private:
    string getTextureType();
    TextureSourceType getTypeFromString(string s);
    void startDoingThingForType();
    bool listenersAdded = false;
};
