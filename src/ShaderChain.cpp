#include "ShaderChain.h"

void ShaderChain::Setup(glm::vec2 res) {
    this->pngRenderer = new PNGRenderer(3.14159, 30, res);
    this->isRunning = true;
    this->gui.setup("Params");
    this->guiGlobal.setup("Global", "GlobalSettings.xml", ofGetWidth()-250, 0);
    this->pngRenderer->AddToGui(&(this->guiGlobal));
    this->pngRenderer->resolutionXParam.addListener(this, &ShaderChain::ResolutionDidChange);
    this->pngRenderer->resolutionYParam.addListener(this, &ShaderChain::ResolutionDidChange);
    this->pngRenderer->savePresetButton.addListener(this, &ShaderChain::WriteToJson);
    this->showGui = true;
    this->fft.Start();
    SetupMidi();
}

ShaderChain::~ShaderChain() {
  delete this->pngRenderer;
  for (uint i = 0; i < this->passes.size(); i++) {
      delete this->passes[i];
  }
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

void ShaderChain::ResolutionDidChange(string &val) {
    this->pngRenderer->resolutionX = std::stof(this->pngRenderer->resolutionXParam);
    this->pngRenderer->resolutionY = std::stof(this->pngRenderer->resolutionYParam);
    for (uint i = 0; i < this->passes.size(); i++) {
        this->passes[i]->UpdateResolution(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY);
    }
}

void ShaderChain::BeginSaveFrames() {
    pngRenderer->Start();
}

void ShaderChain::Update() {
    fft.Update();
    bool capturingThisFrame = pngRenderer->isCapturing;

    if (capturingThisFrame) {
        float deltaTime = 1./pngRenderer->FPS;
        pngRenderer->Tick();
        this->time = this->time + deltaTime;
    }
    else {
        if (this->isRunning) {
            float deltaTime = 1./pngRenderer->FPS;
            this->time = pngRenderer->preview ? fmod(this->time + deltaTime, pngRenderer->duration) : this->time + deltaTime;
        }
        UpdateCamera();
    }

    if (this->isRunning) {
        RenderPasses();
    }

    ofClear(25);

    int idx = this->passes.size()-1;
    float x = ofGetWidth()/2.-this->pngRenderer->resolutionX*0.5*this->pngRenderer->displayScaleParam;
    float y = ofGetHeight()/2.-this->pngRenderer->resolutionY*0.5*this->pngRenderer->displayScaleParam;
    float w = this->pngRenderer->resolutionX*this->pngRenderer->displayScaleParam;
    float h = this->pngRenderer->resolutionY*this->pngRenderer->displayScaleParam;

    this->passes[idx]->buffer.draw(x, y, w, h);

    if (this->showGui) {
        this->gui.draw();
        this->guiGlobal.draw();
        this->passesGui.Draw();
    }

    if (capturingThisFrame) {
        this->pngRenderer->WritePNG(&(this->passes[passes.size()-1]->buffer));
    }
}

void ShaderChain::AddPass(ShaderPass *pass) {
    this->passes.push_back(pass);
}

void ShaderChain::RenderPasses() {
    for (uint i = 0; i < this->passes.size(); i++) {
        if (i > 0) {
          this->passes[i]->SetInputTexture(passes[i-1]->buffer);
        }

        this->passes[i]->Render(this->time, &camera, &fft);
    }
}

void ShaderChain::KeyPressed(int key) {
    if (key == ' ') {
        this->isRunning = !this->isRunning;
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
    for (uint i = 0; i < this->passes.size(); i++) {
        for (uint j = 0; j < this->passes[i]->params.size(); j++) {
            this->passes[i]->params[j]->AddToGui(&gui);
        }
    }
}

void ShaderChain::newMidiMessage(ofxMidiMessage& msg) {
    if(msg.status < MIDI_SYSEX) {
        if(msg.status == MIDI_CONTROL_CHANGE) {
            for (uint j = 0; j < this->passes[0]->params.size(); j++) {
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

void ShaderChain::ReadFromJson(std::string path) {
    bool parsingSuccessful = result.open(path);
    this->pngRenderer->presetNameParam = path;

    if (parsingSuccessful) {
        float rotX = result["camrot"]["x"].asFloat();
        float rotY = result["camrot"]["y"].asFloat();
        float rotZ = result["camrot"]["z"].asFloat();
        this->camera.setOrientation(glm::vec3(rotX, rotY, rotZ));

        float camX = result["campos"]["x"].asFloat();
        float camY = result["campos"]["y"].asFloat();
        float camZ = result["campos"]["z"].asFloat();
        this->camera.setPosition(camX, camY, camZ);

        for (int i = 0; i < result["data"].size(); i++) {
            ShaderPass *pass = new ShaderPass();
            pass->LoadFromJson(result["data"][i], this->pngRenderer->resolutionX, this->pngRenderer->resolutionY);
            this->passes.push_back(pass);
        }
        this->passesGui.Setup(this->passes);
    }
    else {
        ofLogError("ofApp::setup")  << "Failed to parse JSON" << endl;
    }
}

void ShaderChain::WriteToJson() {
    for (uint i = 0; i < this->passes.size(); i++) {
        this->result["data"][i]["shaderName"] = this->passes[i]->filePath;
        this->result["data"][i]["wantsLastBuffer"] = this->passes[i]->wantsLastBuffer;
        this->result["res"]["x"] = this->pngRenderer->resolutionX;
        this->result["res"]["y"] = this->pngRenderer->resolutionY;

        this->result["campos"]["x"] = camera.getX();
        this->result["campos"]["y"] = camera.getY();
        this->result["campos"]["z"] = camera.getZ();

        glm::vec3 rot = camera.getOrientationEulerDeg();
        this->result["camrot"]["x"] = rot.x;
        this->result["camrot"]["y"] = rot.y;
        this->result["camrot"]["z"] = rot.z;

        for (uint j = 0; j < this->passes[i]->params.size(); j++) {
            this->result["data"][i]["parameters"][j]["name"] = this->passes[i]->params[j]->uniform;
                this->passes[i]->params[j]->UpdateJson((this->result["data"][i]["parameters"][j]));
        }
    }

    if (!this->result.save(this->pngRenderer->presetNameParam, true)) {
        ofLogNotice("ShaderChain::WriteToJson") << this->pngRenderer->presetNameParam << " written unsuccessfully.";
    } else {
        ofLogNotice("ShaderChain::WriteToJson") << this->pngRenderer->presetNameParam << " written successfully.";
    }
}
