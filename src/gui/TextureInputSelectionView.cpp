#include "TextureInputSelectionView.h"

TextureInputSelectionView::TextureInputSelectionView() {
    this->panel = gui.addContainer();

    this->openFromFileButton.addListener(this, &TextureInputSelectionView::openFromFileButtonPressed);
    this->openFromWebcamButton.addListener(this, &TextureInputSelectionView::openFromWebcamButtonPressed);
    this->cancelButton.addListener(this, &TextureInputSelectionView::cancelButtonPressed);
    this->openFromAudioButton.addListener(this, &TextureInputSelectionView::audioButtonPressed);
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
    this->panel->add(openFromAudioButton.set("Audio"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    for (unsigned int i = 0; i < passNames.size(); i++) {
        ofParameter<void> *passParameter = new ofParameter<void>();
        passParameter->addListener(this, &TextureInputSelectionView::bufferButtonPressed);
        bufferParameters.push_back(passParameter);
        ofxGuiButton *btn = this->panel->add(passParameter->set(passNames[i]), ofJson({{"type", "fullsize"}, {"text-align", "center"}}) );
        this->bufferButtons.push_back(btn);
    }

    this->panel->add(cancelButton.set("Cancel"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    this->panel->setPosition(ofPoint(ofGetWidth()/2-this->panel->getWidth()/2, ofGetHeight()/2-this->panel->getHeight()/2));

    this->panel->setEnabled(true);
}

void TextureInputSelectionView::hide() {
    this->panel->setEnabled(false);
    ofNotifyEvent(hideEvent);
}

void TextureInputSelectionView::openFromFileButtonPressed() {
    if (!this->panel->isEnabled()) return;

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
    if (!this->panel->isEnabled()) return;

    hide();
    updateWebcam(true);
}

void TextureInputSelectionView::bufferButtonPressed() {
    if (!this->panel->isEnabled()) return;

    for (unsigned int i = 0; i < bufferButtons.size(); i++) {
        if (bufferButtons[i]->isMouseOver()) {
            string s = bufferButtons[i]->getName();
            ofNotifyEvent(selectedBufferChanged, s);
            break;
        }
    }
    hide();
}

void TextureInputSelectionView::updateWebcam(bool val) {
    ofNotifyEvent(wantsWebcamChanged, val);
}

void TextureInputSelectionView::cancelButtonPressed() {
    hide();
}

void TextureInputSelectionView::audioButtonPressed() {
    if (!this->panel->isEnabled()) return;
    updateAudio(true);
    hide();
}

void TextureInputSelectionView::updateAudio(bool val) {
    ofNotifyEvent(wantsAudioChanged, val);
}
