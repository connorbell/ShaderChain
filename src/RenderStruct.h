#pragma once

class FFTManager;
class ShaderPass;

struct RenderStruct {
    float time;
    ofNode *cam;
    FFTManager *fft;
    ofVideoGrabber *vidGrabber;
    std::vector<ShaderPass*> *passes;
};
