#pragma once

#include "Parameter.h"
#include "ofxGuiExtended2.h"

class TextureParameter : public Parameter {

public:
    TextureParameter(string uniform, string filePath, int textureIndex, bool show);
    ofImage value;
    string filePath;
    int textureIndex;
    virtual void UpdateShader(ofxAutoReloadedShader *shader) override;
    virtual void AddToGui(ofxGuiGroup2 *gui) override;
};
