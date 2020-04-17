#include "PNGRenderer.h"
#include <math.h>
#include "ShaderChain.h"

PNGRenderer::PNGRenderer(float duration, int fps, glm::vec2 resolution) {
    this->duration = duration;
    this->FPS = fps;

    this->currentFrame = 0;
    this->totalFrames = duration * fps;
    this->resolutionX = resolution.x;
    this->resolutionY = resolution.y;
    this->filePath = "renders/cool";
    this->displayScaleParam = 1.0;
    this->renderedFrames = 1;
    this->frameskip = 1;
    this->duration = 3.14159;
    this->FPS = 30;
    this->preview = false;
}

void PNGRenderer::AddToGui(ofxGuiPanel *panel) {

    panel->addFpsPlotter();
    panel->add(openFileButton.set("Open File"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    panel->add<ofxGuiFloatInputField>(resolutionX.set("Res x", resolutionX, 0, 4096));
    panel->add<ofxGuiFloatInputField>(resolutionY.set("Res y", resolutionY, 0, 4096));

    panel->add(savePresetButton.set("Save Preset"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    panel->add<ofxGuiTextField>(presetNameParam.set("Preset name", presetNameParam));
    panel->add(displayScaleParam.set("Display scale", displayScaleParam, 0.1, 5.0));

    panel->add<ofxGuiFloatInputField>(duration.set("Duration", duration, 0, 10000000));
    panel->add<ofxGuiIntInputField>(FPS.set("fps", FPS, 0, 1000));
    panel->add(frameskip.set("Frameskip", frameskip, 1, 10));
    panel->add(preview.set("Preview", preview));
    panel->add(saveButton.set("Save Frames"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    this->saveButton.addListener(this, &PNGRenderer::Start);
}

float PNGRenderer::Tick() {
  float progress = (float)this->currentFrame / this->totalFrames;

  this->currentFrame = (this->currentFrame+1) % this->totalFrames;
  this->renderedFrames++;
  cout << this->renderedFrames << endl;
  if (this->renderedFrames == this->totalFrames) {
    this->isCapturing = false;
  }

  return progress * this->duration;
}

void PNGRenderer::WritePNG(ofFbo *buffer) {
  ofPixels outputPixels;
  outputPixels.allocate(this->resolutionX, this->resolutionY, OF_IMAGE_COLOR);
  int totalZeros = (int)floor(log10 (((float)this->totalFrames)));
  int totalZerosNeeded = totalZeros - (int)floor(log10 (((float)this->currentFrame)));
  if (this->currentFrame == 0) totalZerosNeeded = totalZeros;

  std::string s = "";
  for (int i = 0; i < totalZerosNeeded; i++) s += "0";

  s += std::to_string(this->currentFrame);
  buffer->readToPixels(outputPixels);
  ofSaveImage(outputPixels, this->filePath + "_" + s + ".png", OF_IMAGE_QUALITY_BEST);
}

void PNGRenderer::Start() {
    this->renderedFrames = 0;
    this->totalFrames = duration * FPS;
    this->isCapturing = true;
}

void PNGRenderer::UpdateResolution(int w, int h) {
    this->resolutionX = w;
    this->resolutionY = h;
}
