#pragma once
#include <vector>
#include "ofMain.h"
#include "ofxGuiExtended2.h"

class TextureInputSelectionView {

public:
    ofxGui gui;
    ofxGuiContainer *panel;

    ofParameter<void> openFromFileButton;
    ofParameter<void> openFromWebcamButton;
    ofParameter<void> cancelButton;

    vector<ofxGuiButton*> bufferButtons;
    vector<ofParameter<void>*> bufferParameters;
    vector<string> passNames;

    ofEvent<string> selectedFilePathChanged;
    ofEvent<bool> wantsWebcamChanged;
    ofEvent<string> selectedBufferChanged;

    ofEvent<void> hideEvent;

    string selectedFilePath;
    bool isShowingFileDialogue = false;

    TextureInputSelectionView();
    ~TextureInputSelectionView();
    void show();
    void hide();
    void deleteParams();

    void updateWebcam(bool val);
    void openFromFileButtonPressed();
    void openFromWebcamButtonPressed();
    void openFromBufferPressed();
    void bufferButtonPressed();
    void cancelButtonPressed();
};
