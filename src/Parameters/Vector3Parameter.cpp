#include "Vector3Parameter.h"

Vector3Parameter::Vector3Parameter(std::string uniform, glm::vec3 defaultVal, bool show, glm::vec2 range, int *midi) {
  this->uniform = uniform;
  this->value = defaultVal;
  this->show = show;
  this->range = range;
  this->midi = new int[3];
  this->midi[0] = midi[0];
  this->midi[1] = midi[1];
  this->midi[2] = midi[2];
}

void Vector3Parameter::UpdateShader(ofxAutoReloadedShader *shader) {
  shader->setUniform3f(this->uniform, this->value->x, this->value->y, this->value->z);
}

void Vector3Parameter::AddToGui(ofParameterGroup *gui) {
    if (this->show) {
        gui->add(value.set(this->uniform,glm::vec3(this->value->x, this->value->y, this->value->z),glm::vec3(range.x,range.x,range.x),glm::vec3(range.y,range.y,range.y)));
    }
}

void Vector3Parameter::UpdateJson(Json::Value &val) {
    val["value"]["x"] = this->value->x;
    val["value"]["y"] = this->value->y;
    val["value"]["z"] = this->value->z;
    val["show"] = this->show;
    val["type"] = 1;
}

void Vector3Parameter::UpdateMidi(int index, float val) {
    if (this->midi[0] == index) {
        float adjustedValue = ofLerp(this->range.x, this->range.y, val/127.0);
        this->value = glm::vec3(adjustedValue, this->value->y, this->value->z);
    } else if (this->midi[1] == index) {
        float adjustedValue = ofLerp(this->range.x, this->range.y, val/127.0);
        this->value = glm::vec3(this->value->x, adjustedValue, this->value->z);
    } else if (this->midi[2] == index) {
        float adjustedValue = ofLerp(this->range.x, this->range.y, val/127.0);
        this->value = glm::vec3(this->value->x, this->value->y, adjustedValue);
    }
}
