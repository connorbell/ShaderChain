#include "MidiMapper.h"
#include "RenderStruct.h"
#include "ShaderPass.h"

void MidiMapper::show(ofxGui *gui, RenderStruct *renderData) {
    if (container == nullptr) {
        container = gui->addContainer();
    }
    isShowing = true;
    renderStruct = renderData;
    container->clear();
    midiButtons.clear();
    this->container->add(statusLabel.set("Click uniform then midi"));
    for (unsigned int i = 0; i < renderData->passes->size(); i++) {
        for (unsigned j = 0; j < renderData->passes->at(i)->params.size(); j++) {
            if (renderData->passes->at(i)->params[j]->type == "float") {
                ofParameter<void> midiButton;
                ofxGuiButton *btn = this->container->add(midiButton.set(renderData->passes->at(i)->params[j]->uniform), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btn);
            }
            else if (renderData->passes->at(i)->params[j]->type == "vec4") {
                ofParameter<void> midiXButton;
                ofParameter<void> midiYButton;
                ofParameter<void> midiZButton;
                ofParameter<void> midiWButton;

                ofxGuiButton *btnX = this->container->add(midiXButton.set(renderData->passes->at(i)->params[j]->uniform + " x"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnX);
                ofxGuiButton *btnY = this->container->add(midiYButton.set(renderData->passes->at(i)->params[j]->uniform + " y"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnY);
                ofxGuiButton *btnZ = this->container->add(midiZButton.set(renderData->passes->at(i)->params[j]->uniform + " z"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnZ);
                ofxGuiButton *btnW = this->container->add(midiWButton.set(renderData->passes->at(i)->params[j]->uniform + " w"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnW);
            } else if (renderData->passes->at(i)->params[j]->type == "vec3") {
                ofParameter<void> midiXButton;
                ofParameter<void> midiYButton;
                ofParameter<void> midiZButton;

                ofxGuiButton *btnX = this->container->add(midiXButton.set(renderData->passes->at(i)->params[j]->uniform + " x"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnX);
                ofxGuiButton *btnY = this->container->add(midiYButton.set(renderData->passes->at(i)->params[j]->uniform + " y"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnY);
                ofxGuiButton *btnZ = this->container->add(midiZButton.set(renderData->passes->at(i)->params[j]->uniform + " z"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnZ);
            } else if (renderData->passes->at(i)->params[j]->type == "vec2") {
                ofParameter<void> midiXButton;
                ofParameter<void> midiYButton;

                ofxGuiButton *btnX = this->container->add(midiXButton.set(renderData->passes->at(i)->params[j]->uniform + " x"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnX);
                ofxGuiButton *btnY = this->container->add(midiYButton.set(renderData->passes->at(i)->params[j]->uniform + " y"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
                midiButtons.push_back(btnY);
            }
        }
    }

    for (int i = 0; i < midiButtons.size(); i++) {
        midiButtons[i]->addListener(this, &MidiMapper::midiButtonPressed);
    }

    cancelButton.addListener(this,&MidiMapper::cancelButtonPressed);
    this->container->add(cancelButton.set("Close"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    this->container->setPosition(ofPoint(ofGetWidth()/2-this->container->getWidth()/2, ofGetHeight()/2-this->container->getHeight()/2));
    this->container->setEnabled(true);
}

void MidiMapper::midiSet(int val) {
    if (targetUniform == "" || val == -1) return;

    statusLabel = "set " + targetUniform + " to " + to_string(val);

    for (unsigned int i = 0; i < renderStruct->passes->size(); i++) {
        for (unsigned j = 0; j < renderStruct->passes->at(i)->params.size(); j++) {
            if (targetUniform == renderStruct->passes->at(i)->params[j]->uniform) {
                renderStruct->passes->at(i)->params[j]->bindMidi(val, subUniform);
                break;
            }
        }
    }

    targetUniform = "";
    targetIndex = -1;
    subUniform = -1;
}

void MidiMapper::cancelButtonPressed() {
    this->container->setEnabled(false);
    isShowing = false;
}

void MidiMapper::midiButtonPressed() {
    for (unsigned int i = 0; i < midiButtons.size(); i++) {
        if (midiButtons[i]->isMouseOver()) {
            string name = midiButtons[i]->getName();

            if (name.find(' ') != std::string::npos) {
                targetUniform = name.substr(0, name.find_last_of(" "));
                auto sub = name.substr(name.find_last_of(" ") + 1);
                subUniform = indexFromSub(sub);
                statusLabel = "mapping " + targetUniform + "  " + sub;
            } else {
                targetUniform = name;
                statusLabel = "mapping " + name;
            }

            break;
        }
    }
}

int MidiMapper::indexFromSub(string s) {
    if (s == "x") {
        return 0;
    } else if (s == "y") {
        return 1;
    } else if (s == "z") {
        return 2;
    } else if (s == "w") {
        return 3;
    }
}
