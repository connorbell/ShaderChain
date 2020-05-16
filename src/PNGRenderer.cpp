#include "PNGRenderer.h"
#include <math.h>
#include "ShaderChain.h"

PNGRenderer::PNGRenderer(float animduration, int fps, glm::vec2 resolution) {
    this->animduration = animduration;
    this->FPS = fps;

    this->currentFrame = 0;
    this->totalFrames = animduration * fps;
    this->resolutionX = resolution.x;
    this->resolutionY = resolution.y;
    this->filePath = "renders/cool";
    this->displayScaleParam = 1.0;
    this->renderedFrames = 1;
    this->frameskip = 1;
    this->FPS = 30;
    this->numBlendFrames = 1;
    this->preview = false;
}

void PNGRenderer::AddToGui(ofxGuiPanel *panel) {

    panel->add(statusLabel.set("Playing",""));

    panel->addFpsPlotter();
    panel->add(openFileButton.set("Open File"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    panel->add<ofxGuiFloatInputField>(resolutionX.set("Res x", resolutionX, 0, 4096));
    panel->add<ofxGuiFloatInputField>(resolutionY.set("Res y", resolutionY, 0, 4096));

    panel->add(savePresetButton.set("Save Preset"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    panel->add<ofxGuiTextField>(presetNameParam.set("Preset name", "name"));
    panel->add(displayScaleParam.set("Display scale", displayScaleParam, 0.1, 5.0));

    panel->add<ofxGuiFloatInputField>(animduration.set("Duration", animduration, 0, 10000000));
    panel->add<ofxGuiIntInputField>(FPS.set("fps", FPS, 0, 1000));
    panel->add(frameskip.set("Frameskip", frameskip, 1, 10));
    panel->add(preview.set("Preview", preview));

    renderParameterGroup.setName("Rendering");
    renderParameterGroup.add(numBlendFrames.set("Num Blend Frames", numBlendFrames, 1, 128));
    renderParameterGroup.add(saveButton.set("Save Frames"));
    renderParameterGroup.add(encodeMp4Button.set("Encode mp4"));

    gifNumColors = 256;
    gifMenuGroup.setName("Gif");
    gifMenuGroup.add(gifNumColors.set("Colors", gifNumColors, 1, 255));
    gifMenuGroup.add(encodeGifButton.set("Encode gif"));

    renderParameterGroup.add(gifMenuGroup);

    ofxGuiContainer* menu = panel->addMenu(renderParameterGroup);
}

float PNGRenderer::Tick() {
    float progress = (float)this->currentFrame / this->totalFrames;

    this->currentFrame = (this->currentFrame+1) % this->totalFrames;
    this->renderedFrames++;
    this->statusLabel = ("Rendered " + to_string(renderedFrames) + " frames");

    cout << this->renderedFrames << endl;

    if (this->renderedFrames == this->totalFrames) {
        this->isCapturing = false;
    }

    return progress * this->animduration;
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
  string destFilePath = this->filePath + "_" + s + ".png";
  ofSaveImage(outputPixels, destFilePath, OF_IMAGE_QUALITY_BEST);
  cout << destFilePath << endl;
}

void PNGRenderer::Start() {
    string s = presetNameParam.get();
    string file = s.substr(s.find_last_of("/") + 1);
    string fileWithoutExtension = file.substr(0, file.find_last_of("."));
    this->filePath = "renders/" + fileWithoutExtension;
    this->renderedFrames = 0;
    this->totalFrames = animduration * FPS;
    this->isCapturing = true;
}

void PNGRenderer::UpdateResolution(int w, int h) {
    this->resolutionX = w;
    this->resolutionY = h;
}

void saveGif() {
    system(" ");
}

void saveVideo(string outputFilename) {

}
