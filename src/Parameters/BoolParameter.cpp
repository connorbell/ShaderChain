#include "BoolParameter.h"

BoolParameter::BoolParameter(std::string uniform, bool currentValue, bool show, int midi) {
  this->uniform = uniform;
  this->value = currentValue;
  this->range = range;
  this->show = show;
  this->midiIndex = midi;
}

void BoolParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {
  shader->setUniform1f(this->uniform, this->value ? 1 : 0);
}

void BoolParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        gui->add(value.set(this->uniform,this->value));
    }
}

void BoolParameter::UpdateJson(Json::Value &val) {
    val["value"] = (bool)this->value;
    val["show"] = this->show;
    val["type"] = "bool";
}

void BoolParameter::UpdateMidi(int index, float value) {
    if (this->midiIndex == index) {
        //this->value = ofLerp(this->range.x, this->range.y, value/127.0);
    }
}

void BoolParameter::BindMidi(int midiIndices[]) {
    this->midiIndex = midiIndices[0];
}
