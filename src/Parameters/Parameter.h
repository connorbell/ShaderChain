#pragma once
#include <string>

#include "ofMain.h"
#include "ofxGuiExtended2.h"
#include "ofxAutoReloadedShader.h"
#include "ofxJSON.h"
#include "TextureInputSelectionView.h"
#include "RenderStruct.h"

class ShaderPass;

class Parameter {
public:
  std::string uniform;
  bool show;
  TextureInputSelectionView *selectionView;

  virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {}
  virtual void AddToGui(ofxGuiGroup2 *gui) {}
  virtual void UpdateJson(Json::Value &val) {}
  virtual void UpdateMidi(int midiIndex, float value) {}
  virtual void BindMidi(int midiIndices[]) {}
  virtual void update(RenderStruct *renderStruct) {}
  virtual void startOfflineRender() {}
  virtual void stopOfflineRender() {}
};
