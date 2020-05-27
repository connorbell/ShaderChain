#include "ColorParameter.h"

ColorParameter::ColorParameter(std::string uniform, float r, float g, float b, float a, bool show, int *midi) {
  this->uniform = uniform;
  this->value = ofColor(r, g, b, a);
  this->show = show;
  this->midi = new int[4];
  this->midi[0] = midi[0];
  this->midi[1] = midi[1];
  this->midi[2] = midi[2];
  this->midi[3] = midi[3];
}

void ColorParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {
  shader->setUniform4f(this->uniform, (float)this->value->r/255.0, (float)this->value->g/255.0, (float)this->value->b/255.0, (float)this->value->a/255.0);
}

void ColorParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        gui->add(value.set(uniform, value, ofColor(0,0),ofColor(255,255)));
    }
}

void ColorParameter::UpdateJson(Json::Value &val) {
    val["value"]["r"] = this->value->r;
    val["value"]["g"] = this->value->g;
    val["value"]["b"] = this->value->b;
    val["value"]["a"] = this->value->a;
    val["show"] = this->show;
    val["type"] = "color";
}

void ColorParameter::UpdateMidi(int index, float val) {
    if (this->midi[0] == index) {

    } else if (this->midi[1] == index) {

    } else if (this->midi[2] == index) {

    }
}
