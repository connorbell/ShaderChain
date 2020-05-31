#include "FloatParameter.h"

FloatParameter::FloatParameter(std::string uniform, float currentValue, glm::vec2 range, bool show, int midi) {
  this->uniform = uniform;
  this->value = currentValue;
  this->range = range;
  this->show = show;
  this->midiIndex = midi;
}

void FloatParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {
  shader->setUniform1f(this->uniform, this->value);
}

void FloatParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        gui->add(value.set(this->uniform,this->value,this->range.x,this->range.y));
    }
}

void FloatParameter::UpdateJson(Json::Value &val) {
    cout << "update " << this->uniform << " " << this->value << endl;
    val["value"] = (float)this->value;
    val["show"] = this->show;
    val["type"] = "float";
    val["range"]["x"] = this->range.x;
    val["range"]["y"] = this->range.y;
}

void FloatParameter::UpdateMidi(int index, float value) {
    if (this->midiIndex == index) {
        this->value = ofLerp(this->range.x, this->range.y, value/127.0);
    }
}

void FloatParameter::BindMidi(int midiIndices[]) {
    this->midiIndex = midiIndices[0];
}
