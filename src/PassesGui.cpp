#include "PassesGui.h"

PassesGui::PassesGui() {
    this->panel = gui.addPanel();
    passButtons = panel->add<ofxSortableList>("Passes");
    this->panel->setPosition(ofPoint(10, 440));
}

PassesGui::~PassesGui() {

}

void PassesGui::Setup(std::vector<ShaderPass*> *passes) {
    passButtons->clear();
    this->passes = passes;

    for (uint i = 0; i < passes->size(); i++) {
        ofParameter<string> text;
        text.set(passes->at(i)->filePath, "");
        passButtons->add(text);
    }
}

void PassesGui::Draw() {

}
