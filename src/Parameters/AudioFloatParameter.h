#pragma once
#include <string>

#include "Parameter.h"
#include "FloatParameter.h"

class AudioFloatParameter : public FloatParameter {
public:
  glm::vec2 frequencyRange;
  float scaleFactor;
  float expFactor;
  bool accumulate;

  AudioFloatParameter(std::string uniform, float currentValue, glm::vec2 range, glm::vec2 frequencyRange, float scaleFactor, float expFactor, bool accumulate, bool show, int midi);
  void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct);
  void UpdateJson(Json::Value &val);
};
