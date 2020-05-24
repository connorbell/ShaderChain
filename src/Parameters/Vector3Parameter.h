#pragma once
#include <string>

#include "Parameter.h"

class Vector3Parameter : public Parameter {
public:
  ofParameter<ofVec3f> value;
  glm::vec2 range;
  int *midi;

  Vector3Parameter(std::string uniform, ofVec3f defaultVal, bool show, glm::vec2 range, int *midi);
  virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) override;
  virtual void AddToGui(ofxGuiGroup2 *gui) override;
  virtual void UpdateJson(Json::Value &val) override;
  virtual void UpdateMidi(int index, float val) override;
};
