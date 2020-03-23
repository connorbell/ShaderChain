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
}

void PNGRenderer::AddToGui(ofxPanel *panel) {
    std::ostringstream xString;
    xString << resolutionX;

    std::ostringstream yString;
    yString << resolutionY;

    panel->add(resolutionXParam.set("res x",xString.str()));
    panel->add(resolutionYParam.set("res y",yString.str()));

    std::ostringstream durationString;
    durationString << duration;

    std::ostringstream fpsString;
    fpsString << FPS;

    panel->add(savePresetButton.setup("Save Preset"));
    panel->add(presetNameParam.set("preset name", presetNameParam));
    panel->add(displayScaleParam.set("Display resolution", displayScaleParam,1.0, 5.0));

    panel->add(preview.set("Preview", preview));
    panel->add(durationParam.set("duration", durationString.str()));
    panel->add(fpsParam.set("fps", fpsString.str()));
    panel->add(saveButton.setup("Save Frames"));

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
    this->duration = std::stof(durationParam);
    this->FPS = std::stof(fpsParam);
    cout << this->FPS << " " << this->duration << endl;
    this->renderedFrames = 0;
    this->totalFrames = duration * FPS;
    this->isCapturing = true;
}

void PNGRenderer::UpdateResolution(int w, int h) {
    this->resolutionX = w;
    this->resolutionY = h;
    std::ostringstream xString;
    xString << resolutionX;

    std::ostringstream yString;
    yString << resolutionY;

    resolutionXParam = xString.str();
    resolutionYParam = yString.str();
}
