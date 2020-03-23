#pragma once

#include "Parameter.h"

class TextureParameter : public Parameter {

public:
    ofTexture value;
    virtual void UpdateShader(ofxAutoReloadedShader *shader) override;
};
