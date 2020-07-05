#pragma once

class FFTManager;
class ShaderPass;

struct RenderStruct {
    float time;
    int frame;
    int numBlendFrames;
    FFTManager *fft;
    ofVideoGrabber *vidGrabber;
    std::vector<ShaderPass*> *passes;
    bool isOfflineRendering = false;
    ofFbo *lastBuffer; // This is updated throughout the render process for each pass. Probably don't use this.
    glm::vec2 mousePosition;
};
