#include "FFTManager.h"

void FFTManager::Start() {
    fft.setup(1024, OF_FFT_WINDOW_HAMMING);
    fft.setUseNormalization(true);
    audioTexture.allocate(1024,1,GL_RGB);
    isRunning = true;
}

void FFTManager::Update() {
    if (isRunning) {
        fft.update();

        vector<float>& buffer = fft.getBins();
        int n = MIN(1024, buffer.size());

        unsigned char signal[1024*3];
        int audioIndex = 0;

        for (int i = 0; i < n; i+=3) {
            float v = buffer.at(audioIndex)*255; // FFT
            signal[i] = (unsigned char) v ;
            signal[i+1] = (unsigned char)v;
            signal[i+2] = (unsigned char)v;
            audioIndex++;
        }
        audioTexture.loadData(signal, 1024, 1, GL_RGB);
    }
}

void FFTManager::UpdateShader(ofShader *shader, int textureIndex) {
    shader->setUniformTexture("_AudioTexture", audioTexture, textureIndex);
}
