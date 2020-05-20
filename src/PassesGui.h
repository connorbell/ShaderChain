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
    ofxGuiPanel *panel;

private:
    ofxGui gui;
    std::vector<ShaderPass*> *passes;
};
