#include "TextureInputSelectionView.h"

TextureInputSelectionView::TextureInputSelectionView() {
    this->panel = gui.addPanel();
    this->panel->setPosition(ofPoint(ofGetWidth()/2, ofGetHeight()/2));

    this->openFromFileButton.addListener(this, &TextureInputSelectionView::openFromFileButtonPressed);
    this->openFromWebcamButton.addListener(this, &TextureInputSelectionView::openFromWebcamButtonPressed);
    this->panel->setEnabled(false);
}

TextureInputSelectionView::~TextureInputSelectionView() {
    deleteParams();
}

void TextureInputSelectionView::deleteParams() {
    for (unsigned int i = 0; i < bufferParameters.size(); i++) {
        delete bufferParameters[i];
    }
    bufferParameters.clear();
}

void TextureInputSelectionView::show() {
    this->panel->clear();
    deleteParams();
    this->bufferButtons.clear();
    this->panel->add(openFromFileButton.set("Open From File"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    this->panel->add(openFromWebcamButton.set("Webcam"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    for (unsigned int i = 0; i < passNames.size(); i++) {
        ofParameter<void> *passParameter = new ofParameter<void>();
        passParameter->addListener(this, &TextureInputSelectionView::bufferButtonPressed);
        bufferParameters.push_back(passParameter);
        ofxGuiButton *btn = this->panel->add(passParameter->set(passNames[i]), ofJson({{"type", "fullsize"}, {"text-align", "center"}}) );
        this->bufferButtons.push_back(btn);
    }

    this->panel->setEnabled(true);
}

void TextureInputSelectionView::hide() {
    this->panel->setEnabled(false);
    ofNotifyEvent(hideEvent);
}

void TextureInputSelectionView::openFromFileButtonPressed() {
    cout << "Open from file" << endl;

    if (!isShowingFileDialogue) {
        isShowingFileDialogue = true;
        ofFileDialogResult result = ofSystemLoadDialog("Load file",system("pwd"));
        if (result.bSuccess) {
            ofNotifyEvent(selectedFilePathChanged, result.filePath);
        }
        hide();
        isShowingFileDialogue = false;
    }
}

void TextureInputSelectionView::openFromWebcamButtonPressed() {
    bool val = true;
    hide();
    ofNotifyEvent(wantsWebcamChanged, val);
}

void TextureInputSelectionView::bufferButtonPressed() {
    for (unsigned int i = 0; i < bufferButtons.size(); i++) {
        if (bufferButtons[i]->isMouseOver()) {
            string s = bufferButtons[i]->getName();
            ofNotifyEvent(selectedBufferChanged, s);
            break;
        }
    }
    hide();
}
