#include "PassesGui.h"

PassesGui::PassesGui() {
    this->panel = gui.addContainer();
    passButtons = panel->add<ofxSortableList>("Passes");
    this->panel->setPosition(ofPoint(0, 210));
}

PassesGui::~PassesGui() {

}

void PassesGui::Setup(std::vector<ShaderPass*> *passes) {
    passButtons->clear();
    this->passes = passes;
    for (int i = 0; i < passes->size(); i++) {
        ofParameter<string> text;
        text.set(passes->at(i)->displayName);
        passButtons->add(text);
    }
}
