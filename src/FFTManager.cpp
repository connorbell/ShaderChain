#include "FFTManager.h"

FFTManager::FFTManager() {
    fft = nullptr;
    playingState.set("-");
    dampening.set("Dampening", dampening, 0.0, 1.0);
    isMicrophoneEnabled.set("Use Microphone", isMicrophoneEnabled);
    volume.set("Volume", volume, 0.0, 1.0);
    removeTrackButton.set("Remove track");
    this->volume = 1.0;
}

FFTManager::~FFTManager() {
    if (fft != nullptr) {
        delete fft;
    }
}

void FFTManager::StartMicInput() {
    fft = new ofxEasyFft();
    fft->setup(1024, OF_FFT_WINDOW_HAMMING);
    fft->setUseNormalization(true);
    playingState.set("Mic Input");
    currentState = InputStateMicInput;
}

void FFTManager::stopMicInput() {
    if (currentState == InputStateMicInput) {
        currentState = InputStateNone;
        playingState.set("-");
    }
}

void FFTManager::addToGui(ofxGuiContainer *container) {
    parameterGroup.setName("Audio");

    audioMenu = container->addMenu(parameterGroup);

    this->dampening = 0.85;
    this->isMicrophoneEnabled = false;

    reloadAudioMenu();
    this->volume.addListener(this, &FFTManager::volumeToggled);
    this->isMicrophoneEnabled.addListener(this, &FFTManager::micToggled);
    this->removeTrackButton.addListener(this, &FFTManager::removeAudioTrack);
}

void FFTManager::reloadAudioMenu() {
    if (audioMenu != nullptr) {
        audioMenu->clear();
        audioMenu->add(playingState);
        audioMenu->add(dampening);
        audioMenu->add(volume);
        audioMenu->add(isMicrophoneEnabled);
        removeTrackGuiButton = audioMenu->add(removeTrackButton, ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
        removeTrackGuiButton->setEnabled(false);
    }
}

void FFTManager::Update() {

    if (currentState == InputStateMicInput) {
        fft->update();

        vector<float>& buffer = fft->getBins();
        int n = MIN(numSamples, buffer.size());
        unsigned char signal[512*3];
        int audioIndex = 0;
        for (int i = 0; i < numSamples*3; i+=3) {
            float v = MIN(255,buffer.at(audioIndex%buffer.size())) * 255;
            signal[i] = (unsigned char) v;
            signal[i+1] = (unsigned char)v;
            signal[i+2] = (unsigned char)v;
            audioIndex++;
        }
        audioTexture.loadData(signal, numSamples, 1, GL_RGB);

        int lastBufferIndex = 0;
        for (int i = 0; i < numSamples*3; i+=3) {
            lastBuffer[lastBufferIndex] = signal[i];
            lastBufferIndex++;
        }
    }

    else if (currentState == InputStateSoundFile) {
        if (isPaused) {
            return;
        }

        float *buffer = ofSoundGetSpectrum(numSamples);
        unsigned char signal[512*3];
        int audioIndex = 0;
        for (int i = 0; i < numSamples*3; i+=3) {
            float v = MIN(255,buffer[audioIndex] * 255 + (float)lastBuffer[audioIndex] * dampening ); // FFT
            signal[i] = (unsigned char) v;
            signal[i+1] = (unsigned char) v;
            signal[i+2] = (unsigned char) v;
            audioIndex++;
        }

        audioTexture.loadData(signal, numSamples, 1, GL_RGB);

        int lastBufferIndex = 0;
        for (int i = 0; i < numSamples*3; i+=3) {
            lastBuffer[lastBufferIndex] = signal[i];
            lastBufferIndex++;
        }
    }
}

void FFTManager::UpdateShader(ofShader *shader, int textureIndex) {
    shader->setUniformTexture("_AudioTexture", audioTexture, textureIndex);
}

void FFTManager::loadSoundFile(string filePath) {
    soundFilePath = filePath;
    audioTexture.allocate(numSamples,1,GL_RGB);
    soundPlayer.load(filePath);
    soundPlayer.play();

    soundPlayer.setPosition(0.5);
    soundFileDuration = (soundPlayer.getPositionMS() / 1000) * 2.0;

    cout << "duration " << soundFileDuration << endl;
    soundPlayer.setPosition(0.0);

    soundPlayer.play();
    soundPlayer.setLoop(true);
    currentState = InputStateSoundFile;
    isPaused = false;

    playingState.set("Playing audio input");
    removeTrackGuiButton->setEnabled(true);

}

void FFTManager::removeAudioTrack() {
    if (soundPlayer.isLoaded()) {
        playingState.set("-");
        soundPlayer.stop();
        soundPlayer.unload();
        currentState = InputStateNone;
        removeTrackGuiButton->setEnabled(false);
    }
}

void FFTManager::setPaused(bool val) {
    soundPlayer.setPaused(val);
    isPaused = val;
}

void FFTManager::setTime(float time) {
    //soundPlayer.setPaused(true);

    if (currentState != InputStateNone) {
        float t = fmod(time,soundFileDuration) * 1000.;
        soundPlayer.setPositionMS(t);
    }
}

void FFTManager::resetSongIfPlaying() {
    setTime(0.0);
}

void FFTManager::volumeToggled(float &val) {
    soundPlayer.setVolume(val);
}

void FFTManager::micToggled(bool &val) {
    if (val) {
        StartMicInput();
    } else {
        stopMicInput();
    }
}
