#pragma once
#include <string>

#include "Parameter.h"

class Vector3Parameter : public Parameter {
public:
  ofParameter<glm::vec3> value;
  glm::vec2 range;
  int *midi;

  Vector3Parameter(std::string uniform, glm::vec3 defaultVal, bool show, glm::vec2 range, int *midi);
  virtual void UpdateShader(ofxAutoReloadedShader *shader) override;
  virtual void AddToGui(ofParameterGroup *gui) override;
  virtual void UpdateJson(Json::Value &val) override;
  virtual void UpdateMidi(int index, float val) override;
};
