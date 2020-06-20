#pragma once

#include "ofxEasyFft.h"
#include "ofxGuiExtended2.h"

typedef enum {
        InputStateNone,
        InputStateMicInput,
        InputStateSoundFile
} InputState;

class FFTManager {
public:
    FFTManager();
    ~FFTManager();

    int numSamples = 512;

    ofxEasyFft *fft;
    ofTexture audioTexture;
    unsigned char lastBuffer[1024];
    ofSoundPlayer soundPlayer;
    InputState currentState = InputStateNone;
    string soundFilePath;

    ofParameterGroup parameterGroup;
    ofParameter<string> playingState;
    ofParameter<float> volume;
    ofParameter<bool> isMicrophoneEnabled;
    ofParameter<float> dampening;

    void Stop();
    void Update();
    void StartMicInput();
    void loadSoundFile(string filePath);
    void UpdateShader(ofShader *shader, int textureIndex);
    void setPaused(bool val);
    void setTime(float time);
    void resetSongIfPlaying();
    void addToGui(ofxGuiContainer *container);

private:
    bool isPaused = true;
    float soundFileDuration = 0.0;

    void micToggled(bool &val);
    void volumeToggled(float &val);
    void stopMicInput();
};
