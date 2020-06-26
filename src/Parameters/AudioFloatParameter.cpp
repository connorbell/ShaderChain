#include "AudioFloatParameter.h"

AudioFloatParameter::AudioFloatParameter(std::string uniform, float currentValue, glm::vec2 range, glm::vec2 frequencyRange, float scaleFactor, float expFactor, bool accumulate, bool show, int midi)
  : FloatParameter(uniform, currentValue, range, show, midi) {
    this->uniform = uniform;
    this->value = currentValue;
    this->range = range;
    this->frequencyRange = frequencyRange;
    this->scaleFactor = scaleFactor;
    this->expFactor = expFactor;
    this->accumulate = accumulate;
    this->show = show;
    this->midiIndex = midi;
  }

void AudioFloatParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {
  float *buffer = ofSoundGetSpectrum(1024);
  int binx = (int)(this->frequencyRange.x*1024/22050);
  int biny = (int)(this->frequencyRange.y*1024/22050);
  float sum = 0;

  // sum the bins that correspond to the specified frequency range
  for(int i=binx;i<biny;i++) {
    sum += buffer[i];
  }
  // divide by the total number of bins to get the average volume
  // then apply exponential scaling and finally linear scaling coefficient
  sum = pow(sum/(biny-binx), this->expFactor)*this->scaleFactor;

  // either accumulate, OR scale value (roughly) to range, then clip
  if(this->accumulate) {
    this->value += sum;
    while(this->value > this->range.y) this->value -= (this->range.y-this->range.x);
  } else {
    this->value = sum*(this->range.y - this->range.x) + this->range.x;
    this->value = this->value > this->range.y ? this->range.y : (float)this->value;
    this->value = this->value < this->range.x ? this->range.x : (float)this->value;
  }
  shader->setUniform1f(this->uniform, this->value);
}

void AudioFloatParameter::UpdateJson(Json::Value &val) {
    val["show"] = this->show;
    val["type"] = "audioFloat";
    val["range"]["x"] = this->range.x;
    val["range"]["y"] = this->range.y;
    val["frequencyRange"]["x"] = this->frequencyRange.x;
    val["frequencyRange"]["y"] = this->frequencyRange.y;
    val["scaleFactor"] = this->scaleFactor;
    val["expFactor"] = this->expFactor;
    val["accumulate"] = this->accumulate;
}
