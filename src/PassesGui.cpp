#include "PassesGui.h"

PassesGui::PassesGui() {
    this->gui.setup("Passes");
}

void PassesGui::Setup(std::vector<ShaderPass*> passes) {
    gui.clear();

    for (uint i = 0; i < passButtons.size(); i++) {
        delete passButtons[i];
    }
    if (passButtons.size() > 0) {
        passButtons.clear();
    }

    for (uint i = 0; i < passes.size(); i++) {
        ofxButton *button = new ofxButton();
        button->setup(passes[i]->filePath);
        passButtons.push_back(button);

        gui.add(button);
    }
}

void PassesGui::Draw() {
    this->gui.draw();
}
