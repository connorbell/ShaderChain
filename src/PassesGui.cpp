#include "PassesGui.h"

void PassesGui::Setup(std::vector<ShaderPass*> passes) {
    this->gui.setup("Passes");

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
