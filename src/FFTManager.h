#include "ofxEasyFft.h"

class FFTManager {
public:
    ofxEasyFft fft;
    ofTexture audioTexture;
    bool isRunning;

    void Start();
    void Stop();
    void Update();
    void UpdateShader(ofShader *shader, int textureIndex);
};
