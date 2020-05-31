#include "IntParameter.h"

IntParameter::IntParameter(std::string uniform, int currentValue, glm::vec2 range, bool show, int midi) {
  this->uniform = uniform;
  this->value = currentValue;
  this->range = range;
  this->show = show;
  this->midiIndex = midi;
}

void IntParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {
  shader->setUniform1f(this->uniform, this->value);
}

void IntParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        gui->add(value.set(this->uniform,this->value,(int)this->range.x,(int)this->range.y));
    }
}

void IntParameter::UpdateJson(Json::Value &val) {
    val["value"] = (int)this->value;
    val["show"] = this->show;
    val["type"] = "int";
    val["range"]["x"] = this->range.x;
    val["range"]["y"] = this->range.y;
}

void IntParameter::UpdateMidi(int index, float value) {
    if (this->midiIndex == index) {
        //this->value = ofLerp(this->range.x, this->range.y, value/127.0);
    }
}

void IntParameter::BindMidi(int midiIndices[]) {
    this->midiIndex = midiIndices[0];
}
