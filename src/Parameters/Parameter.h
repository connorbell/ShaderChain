#pragma once
#include <string>

#include "ofMain.h"
#include "ofxGuiExtended2.h"
#include "ofxAutoReloadedShader.h"
#include "ofxJSON.h"
#include "TextureInputSelectionView.h"
#include "RenderStruct.h"

typedef enum {
    ImageFile,
    VideoFile,
    Buffer,
    Webcam,
    Last,
    Audio,
    None
} TextureSourceType;

class ShaderPass;

class Parameter {
public:
  std::string uniform;
  bool show;
  TextureInputSelectionView *selectionView;
  string parentBufferName;

  virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {}
  virtual void AddToGui(ofxGuiGroup2 *gui) {}
  virtual void UpdateJson(Json::Value &val) {}
  virtual void UpdateMidi(int midiIndex, float value) {}
  virtual void BindMidi(int midiIndices[]) {}
  virtual void update(RenderStruct *renderStruct) {}
  virtual void startOfflineRender() {}
  virtual void stopOfflineRender() {}
  virtual bool isMouseHoveredOver() { return false; }
  virtual void handleInputFile(string s) {}
  virtual bool getTextureSourceType() { return None; }
  virtual void playbackDidToggleState(bool isPaused) {}
};
