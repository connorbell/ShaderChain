#include "Vector2Parameter.h"

Vector2Parameter::Vector2Parameter(std::string uniform, ofVec2f defaultVal, bool show, glm::vec2 range, int *midi) {
  this->uniform = uniform;
  this->value = defaultVal;
  this->show = show;
  this->range = range;
  this->midi = new int[2];
  this->midi[0] = midi[0];
  this->midi[1] = midi[1];
}

void Vector2Parameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {
  shader->setUniform2f(this->uniform, this->value->x, this->value->y);
}

void Vector2Parameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        value.set(this->uniform,this->value, ofVec2f(range.x,range.x),ofVec3f(range.y,range.y));
        gui->add(value);
    }
}

void Vector2Parameter::UpdateJson(Json::Value &val) {
    val["value"]["x"] = this->value->x;
    val["value"]["y"] = this->value->y;
    val["range"]["x"] = this->range.x;
    val["range"]["y"] = this->range.y;
    val["show"] = this->show;
    val["type"] = "vec2";
}

void Vector2Parameter::UpdateMidi(int index, float val) {
    if (this->midi[0] == index) {
        float adjustedValue = ofLerp(this->range.x, this->range.y, val/127.0);
        this->value = glm::vec2(adjustedValue, this->value->y);
    } else if (this->midi[1] == index) {
        float adjustedValue = ofLerp(this->range.x, this->range.y, val/127.0);
        this->value = glm::vec2(this->value->x, adjustedValue);
    }
}
