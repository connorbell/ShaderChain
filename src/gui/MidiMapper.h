#pragma once
#include "ofxGuiExtended2.h"
class RenderStruct;

class MidiMapper {

public:
    void show(ofxGui *gui, RenderStruct *renderData);
    ofxGuiContainer *container = nullptr;
    bool isShowing = false;
    void midiSet(int val);

private:
    ofParameter<void> cancelButton;
    ofParameter<string> statusLabel;
    void cancelButtonPressed();
    std::vector<ofxGuiButton*> midiButtons;
    void midiButtonPressed();
    RenderStruct *renderStruct;
    string targetUniform = "";
    int targetIndex = -1;
    int subUniform = -1;

    int indexFromSub(string s);
};
