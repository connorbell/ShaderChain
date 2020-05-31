#include "PassesGui.h"

PassesGui::PassesGui() {
    this->panel = gui.addPanel();
    passButtons = panel->add<ofxSortableList>("Passes");
    this->panel->setPosition(ofPoint(10, 280));
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
