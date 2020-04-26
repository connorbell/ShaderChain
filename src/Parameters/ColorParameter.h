#pragma once
#include <string>

#include "Parameter.h"

class ColorParameter : public Parameter {
public:
  ofParameter<ofColor> value;
  int *midi;

  ColorParameter(std::string uniform, float r, float g, float b, float a, bool show, int *midi);
  virtual void UpdateShader(ofxAutoReloadedShader *shader) override;
  virtual void AddToGui(ofxGuiGroup2 *gui) override;
  virtual void UpdateJson(Json::Value &val) override;
  virtual void UpdateMidi(int index, float val) override;
};
