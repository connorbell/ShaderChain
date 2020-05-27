#pragma once
#include <string>

#include "Parameter.h"

class IntParameter : public Parameter {
public:
  ofParameter<int> value;
  glm::vec2 range;
  int midiIndex;

  IntParameter(std::string uniform, int currentValue, glm::vec2 range, bool show, int midi);
  virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) override;
  virtual void AddToGui(ofxGuiGroup2 *gui) override;
  virtual void UpdateJson(Json::Value &val) override;
  virtual void UpdateMidi(int index, float value) override;
  virtual void BindMidi(int midiIndices[]) override;
};
