#include "ShaderChain.h"
#include "ofxSortableList.h"

void ShaderChain::Setup(glm::vec2 res) {
    this->passesGui = new PassesGui();
    ofAddListener(passesGui->passButtons->elementRemoved, this, &ShaderChain::removed);
    ofAddListener(passesGui->passButtons->elementMoved, this, &ShaderChain::moved);
    this->pngRenderer = new PNGRenderer(3.14159, 30, res);
    this->isRunning.set("isRunning", true);
    this->isRunning.addListener(this, &ShaderChain::playingChanged);
    this->guiGlobal = gui.addPanel();
    this->pngRenderer->AddToGui(this->guiGlobal);
    this->pngRenderer->savePresetButton.addListener(this, &ShaderChain::WriteToJson);
    this->pngRenderer->openFileButton.addListener(this, &ShaderChain::OpenFilePressed);
    this->pngRenderer->saveButton.addListener(this, &ShaderChain::BeginSaveFrames);
    this->pngRenderer->encodeMp4Button.addListener(this, &ShaderChain::encodeMp4Pressed);
    this->pngRenderer->encodeGifButton.addListener(this, &ShaderChain::encodeGifPressed);
    this->showGui = true;
    this->isMouseDown = false;
    this->isShowingFileDialogue = false;
    this->frame = 0;
    this->parameterPanel = gui.addPanel();
    this->parameterPanel->setPosition(ofPoint(ofGetWidth()-220, 10));
    ofDisableAlphaBlending();
    SetupMidi();
}

ShaderChain::~ShaderChain() {
  delete this->pngRenderer;
  for (unsigned int i = 0; i < this->passes.size(); i++) {
      delete this->passes[i];
  }
  ofRemoveListener(passesGui->passButtons->elementRemoved, this, &ShaderChain::removed);
  ofRemoveListener(passesGui->passButtons->elementMoved, this, &ShaderChain::moved);
  delete this->passesGui;
}

void ShaderChain::SetupMidi() {
    midiIn.listInPorts();
    // open port by number (you may need to change this)
    midiIn.openPort(1);

    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);

    // add ofApp as a listener
    midiIn.addListener(this);
}

void ShaderChain::UpdateResolutionIfChanged() {

    bool needsUpdate = false;

    if (this->passes.size() > 0) {
        if (this->passes[0]->targetResolution.x != this->pngRenderer->resolutionX ||
            this->passes[0]->targetResolution.y != this->pngRenderer->resolutionY) {
            needsUpdate = true;
        }
    }

    if (needsUpdate) {
        for (unsigned int i = 0; i < this->passes.size(); i++) {
            this->passes[i]->UpdateResolution(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY);
        }
    }
}

void ShaderChain::BeginSaveFrames() {

    if (fft.currentState == InputStateSoundFile) {
        this->time = 0.0;
    }

    pngRenderer->Start();
}

void ShaderChain::Update() {
    UpdateResolutionIfChanged();

    bool capturingThisFrame = pngRenderer->isCapturing;

    if (capturingThisFrame) {
        float deltaTime = 1./pngRenderer->FPS;
        pngRenderer->Tick();
        this->time = this->time + deltaTime;
        fft.setTime(this->time);
    }
    else {
        if (this->isRunning) {
            float deltaTime = 1./pngRenderer->FPS;
            this->time = pngRenderer->preview ? fmod(this->time + deltaTime, pngRenderer->animduration) : this->time + deltaTime;
        }
        UpdateCamera();
    }

    fft.Update();

    ofClear(25);

    if (this->isRunning && frame % pngRenderer->frameskip == 0) {
        RenderPasses();
    }
    ofClear(25);

    if (this->passes.size() > 0) {
        int idx = this->passes.size()-1;
        float x = ofGetWidth()/2.-this->pngRenderer->resolutionX*0.5*this->pngRenderer->displayScaleParam;
        float y = ofGetHeight()/2.-this->pngRenderer->resolutionY*0.5*this->pngRenderer->displayScaleParam;
        float w = this->pngRenderer->resolutionX*this->pngRenderer->displayScaleParam;
        float h = this->pngRenderer->resolutionY*this->pngRenderer->displayScaleParam;

        this->passes[idx]->buffer.draw(x, y, w, h);
    }

    if (this->showGui) {
    //    this->gui.draw();
    //    this->guiGlobal.draw();
        this->passesGui->Draw();
    }

    if (capturingThisFrame) {
        this->pngRenderer->WritePNG(&(this->passes[passes.size()-1]->buffer));
    }
    frame++;
}

void ShaderChain::AddPass(ShaderPass *pass) {
    this->passes.push_back(pass);
}

void ShaderChain::RenderPasses() {
    for (int i = 0; i < this->passes.size(); i++) {
        if (i > 0) {
            this->passes[i]->Render(&(passes[i-1]->buffer), this->time, &camera, &fft);
        }
        else {
            this->passes[i]->Render(nullptr, this->time, &camera, &fft);
        }
    }
}
void ShaderChain::dragEvent(ofDragInfo info) {
    if (info.files.size() > 0) {
        processFileInput(info.files[0]);
    }
}

void ShaderChain::KeyPressed(int key) {
    if (key == ' ') {
        this->isRunning = !this->isRunning;
        fft.setPaused(!this->isRunning);
    }
    if (key == 'g') {
        this->showGui = !this->showGui;
    }
    if (key == 'f') {
        ofToggleFullscreen();
    }
    if (key == 'w') {
        camera.move(-camera.getLookAtDir() * ofGetLastFrameTime());
    }
    if (key == 's') {
        camera.move(camera.getLookAtDir() * ofGetLastFrameTime());
    }
    if (key == 'd') {
        camera.move(camera.getXAxis() * ofGetLastFrameTime());
    }
    if (key == 'a') {
        camera.move(-camera.getXAxis() * ofGetLastFrameTime());
    }
}

void ShaderChain::SetupGui() {
    parameterPanel->clear();
    this->parameterPanel->setPosition(ofPoint(ofGetWidth()-220, 10));
    for (int i = 0; i < this->passes.size(); i++) {
        this->passes[i]->AddToGui(parameterPanel);
    }
}

void ShaderChain::newMidiMessage(ofxMidiMessage& msg) {
    if(msg.status < MIDI_SYSEX) {
        if(msg.status == MIDI_CONTROL_CHANGE) {
            for (int j = 0; j < this->passes[0]->params.size(); j++) {
                this->passes[0]->params[j]->UpdateMidi(msg.control, msg.value);
            }
        }
    }
}

void ShaderChain::UpdateCamera() {
    if (isMouseDown) {
        float xDelta = ofGetMouseX() - ofGetPreviousMouseX();
        float yDelta = ofGetMouseY() - ofGetPreviousMouseY();

        camera.panDeg(xDelta * ofGetLastFrameTime() * mouseMoveSpeed);
        camera.tiltDeg(yDelta * ofGetLastFrameTime() * mouseMoveSpeed);
    }
}

void ShaderChain::ReadFromJson(std::string filepath) {
    bool parsingSuccessful = result.open(filepath);
    this->pngRenderer->presetNameParam = filepath;

    for (int i = 0; i < this->passes.size(); i++) {
        delete this->passes[i];
    }
    this->passes.clear();

    if (parsingSuccessful) {
        float rotX = result["camrot"]["x"].asFloat();
        float rotY = result["camrot"]["y"].asFloat();
        float rotZ = result["camrot"]["z"].asFloat();
        this->camera.setOrientation(glm::vec3(rotX, rotY, rotZ));

        float camX = result["campos"]["x"].asFloat();
        float camY = result["campos"]["y"].asFloat();
        float camZ = result["campos"]["z"].asFloat();
        this->camera.setPosition(camX, camY, camZ);

        this->pngRenderer->resolutionX = result["res"]["x"].asFloat();
        this->pngRenderer->resolutionY = result["res"]["y"].asFloat();

        for (int i = 0; i < result["data"].size(); i++) {
            ShaderPass *pass = new ShaderPass();
            pass->LoadFromJson(result["data"][i], this->pngRenderer->resolutionX, this->pngRenderer->resolutionY);
            this->passes.push_back(pass);
        }
        this->passesGui->Setup(&this->passes);
        SetupGui();
    }
    else {
        ofLogError("ofApp::setup")  << "Failed to parse JSON" << endl;
    }
}
void ShaderChain::LoadPassFromFile(string filepath) {
    auto relativeFileName = filepath.substr(filepath.find("data") + 5);
    auto relativeFileNameWithoutExtension = relativeFileName.substr(0,relativeFileName.find("frag")-1);

    ShaderPass *pass = new ShaderPass(relativeFileNameWithoutExtension, glm::vec2(this->pngRenderer->resolutionX,this->pngRenderer->resolutionY) );
    pass->LoadJsonParametersFromLoadedShader();
    this->passes.push_back(pass);
    SetupGui();
    this->passesGui->Setup(&this->passes);
}

void ShaderChain::processFileInput(string filePath) {
    auto extension = filePath.substr(filePath.find_last_of(".") + 1);
    if (extension == "frag") {
        LoadPassFromFile(filePath);
    } else if (extension == "json") {
        ReadFromJson(filePath);
    } else if (extension == "mp3") {
        fft.loadSoundFile(filePath);
    }
}

void ShaderChain::WriteToJson() {
    this->result.clear();
    this->result["res"]["x"] = (float)this->pngRenderer->resolutionX;
    this->result["res"]["y"] = (float)this->pngRenderer->resolutionY;

    this->result["campos"]["x"] = camera.getX();
    this->result["campos"]["y"] = camera.getY();
    this->result["campos"]["z"] = camera.getZ();

    glm::vec3 rot = camera.getOrientationEulerDeg();
    this->result["camrot"]["x"] = rot.x;
    this->result["camrot"]["y"] = rot.y;
    this->result["camrot"]["z"] = rot.z;

    for (int i = 0; i < this->passes.size(); i++) {
        this->result["data"][i]["shaderName"] = this->passes[i]->filePath;
        if (this->passes[i]->lastBufferTextureIndex != -1) {
            this->result["data"][i]["lastBufferTextureIndex"] = this->passes[i]->lastBufferTextureIndex;
        }
        if (this->passes[i]->audioTextureIndex != -1) {
            this->result["data"][i]["audioTextureIndex"] = this->passes[i]->audioTextureIndex;
        }
        this->result["data"][i]["wantsCamera"] = this->passes[i]->wantsCamera;

        for (int j = 0; j < this->passes[i]->params.size(); j++) {
            this->result["data"][i]["parameters"][j]["name"] = this->passes[i]->params[j]->uniform;
            this->passes[i]->params[j]->UpdateJson((this->result["data"][i]["parameters"][j]));
        }
    }

    if (!this->result.save((this->pngRenderer->presetNameParam.get()), true)) {
        ofLogNotice("ShaderChain::WriteToJson") << this->pngRenderer->presetNameParam << " written unsuccessfully.";
    } else {
        ofLogNotice("ShaderChain::WriteToJson") << this->pngRenderer->presetNameParam << " written successfully.";
    }
}

void ShaderChain::removed(RemovedElementData& data) {
	cout << "removed " + ofToString(data.index) + "\n";
    passes.erase(passes.begin() + data.index);
    SetupGui();
}

void ShaderChain::moved(MovingElementData &data) {
    iter_swap(passes.begin() + data.old_index, passes.begin() + data.new_index);
	cout << "moved " + data.name + " from index " + ofToString(data.old_index) + " to index " + ofToString(data.new_index) + "\n";
    SetupGui();
}

void ShaderChain::OpenFilePressed() {

    if (!this->isShowingFileDialogue) {
        this->isShowingFileDialogue = true;
        ofFileDialogResult result = ofSystemLoadDialog("Load file",system("pwd"));

        if (result.bSuccess) {
            processFileInput(result.filePath);
        }
        this->isShowingFileDialogue = false;
    }
}

void ShaderChain::saveVideo(string outputFilename) {
    string f = outputFilename;

    int totalFrames = pngRenderer->FPS * pngRenderer->animduration;
    outputFilename = "data/renders/" + outputFilename;

    string mkdirCommand = "mkdir " + outputFilename;
    system(mkdirCommand.c_str());
    string moveFilesCommand = "mv " + outputFilename + "_*.png " + outputFilename;
    system(moveFilesCommand.c_str());
    outputFilename = outputFilename + "/" + f;

    cout << "Creating mp4 " << outputFilename << endl;
    string outputMp4Filename = outputFilename + ".mp4";
    string fpsString = to_string(pngRenderer->FPS);
    string totalZerosString = to_string((int)floor(log10 (((float)totalFrames)))+1);

    string ffmpegCommand = "ffmpeg -r " + fpsString + " -f image2 -s 1080x1920 -i \"" + outputFilename + "_%0" + totalZerosString + "d.png\" -vcodec libx264 -crf 18 -pix_fmt yuv420p " + outputMp4Filename;
    system(ffmpegCommand.c_str());

    cout << ffmpegCommand << endl;

    if (fft.currentState == InputStateSoundFile) {
        string outputMp4AudioFilename = outputFilename + "_audio.mp4";
        string addSoundCommand = "ffmpeg -i " + outputMp4Filename + " -i \"" + fft.soundFilePath + "\" -vcodec copy -acodec aac -shortest " + outputMp4AudioFilename;
        system(addSoundCommand.c_str());
    }

    updateStatusText("Video saved to " + outputMp4Filename);
}

void ShaderChain::updateStatusText(string s) {
    this->pngRenderer->statusLabel.set(s, "");
}

void ShaderChain::playingChanged(bool &val) {
    if (val) {
        updateStatusText("Playing");
    } else {
        updateStatusText("Paused");
    }
}

void ShaderChain::encodeMp4Pressed() {
    string s = pngRenderer->presetNameParam.get();
    string file = s.substr(s.find_last_of("/") + 1);
    string fileWithoutExtension = file.substr(0, file.find_last_of("."));
    saveVideo(fileWithoutExtension);
}

void ShaderChain::encodeGifPressed() {

}
