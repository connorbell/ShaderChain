#pragma once

#include "ofxEasyFft.h"

typedef enum {
        InputStateNone,
        InputStateMicInput,
        InputStateSoundFile
} InputState;

class FFTManager {
public:
    ofxEasyFft *fft;
    ofTexture audioTexture;
    unsigned char lastBuffer[3072];
    ofSoundPlayer soundPlayer;
    InputState currentState = InputStateNone;
    string soundFilePath;
    FFTManager();
    ~FFTManager();

    void Start();
    void Stop();
    void Update();
    void StartMicInput();
    void loadSoundFile(string filePath);
    void UpdateShader(ofShader *shader, int textureIndex);
    void setPaused(bool val);
    void setTime(float time);
    void resetSongIfPlaying();
    
private:
    bool isPaused = true;
    float soundFileDuration = 0.0;
};
