#pragma once

class FFTManager;
class ShaderPass;

struct RenderStruct {
    float time;
    int frame;
    ofNode *cam;
    FFTManager *fft;
    ofVideoGrabber *vidGrabber;
    std::vector<ShaderPass*> *passes;
    bool isOfflineRendering = false;
};
