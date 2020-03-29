#pragma once

#include "ofxEasyFft.h"

class FFTManager {
public:
    ofxEasyFft fft;
    ofTexture audioTexture;
    bool isRunning;
    unsigned char lastBuffer[3072];

    void Start();
    void Stop();
    void Update();
    void UpdateShader(ofShader *shader, int textureIndex);
};
