#pragma once

#include <vector>
#include "ShaderPass.h"
#include "ofxGuiExtended2.h"
#include "ofxSortableList.h"

class PassesGui {
public:

    PassesGui();
    ~PassesGui();

    ofxSortableList *passButtons;
    ofxGuiButton addPassButton;

    void Setup(std::vector<ShaderPass*> *passes);
    void Draw();

private:
    ofxGui gui;
    ofxGuiPanel *panel;
    std::vector<ShaderPass*> *passes;
};
