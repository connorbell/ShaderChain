#pragma once
#include <string>

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"
#include "ofxJSON.h"

class ShaderPass;

class Parameter {
public:
  std::string uniform;
  bool show;

  virtual void UpdateShader(ofxAutoReloadedShader *shader) {}
  virtual void AddToGui(ofxPanel *gui) {}
  virtual void UpdateJson(Json::Value &val) {}
  virtual void UpdateMidi(int midiIndex, float value) {}
  virtual void BindMidi(int midiIndices[]) {}
};
