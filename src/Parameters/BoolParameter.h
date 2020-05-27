#pragma once
#include <string>

#include "Parameter.h"

class BoolParameter : public Parameter {
public:
  ofParameter<bool> value;
  glm::vec2 range;
  int midiIndex;

  BoolParameter(std::string uniform, bool currentValue, bool show, int midi);
  virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) override;
  virtual void AddToGui(ofxGuiGroup2 *gui) override;
  virtual void UpdateJson(Json::Value &val) override;
  virtual void UpdateMidi(int index, float value) override;
  virtual void BindMidi(int midiIndices[]) override;
};
