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
	this->time = 0.0;
    this->parameterPanel = gui.addPanel();
    this->cumulativeShader.load("shadersGL3/internal/cumulativeAdd");
    this->parameterPanel->setPosition(ofPoint(ofGetWidth()-220, 10));
    ofFloatColor black;

    cumulativeBuffer.allocate(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY, GL_RGBA32F);
    cumulativeBufferSwap.allocate(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY, GL_RGBA32F);
    cumulativeBufferSwap.clearColorBuffer(black);
    cumulativeBuffer.clearColorBuffer(black);

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

void ShaderChain::UpdateResolutionIfChanged(bool force) {

    bool needsUpdate = force;

    if (this->passes.size() > 0) {
        if (this->passes[0]->targetResolution.x != this->pngRenderer->resolutionX ||
            this->passes[0]->targetResolution.y != this->pngRenderer->resolutionY) {
            needsUpdate = true;
        }
    }

    if (needsUpdate) {
        ofSetFrameRate(pngRenderer->FPS);
        ofFloatColor black;

        cumulativeBuffer.allocate(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY, GL_RGBA32F);
        cumulativeBufferSwap.allocate(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY, GL_RGBA32F);
        cumulativeBufferSwap.clearColorBuffer(black);
        cumulativeBuffer.clearColorBuffer(black);

        this->cumulativeRenderPlane.set(pngRenderer->resolutionX, pngRenderer->resolutionY, 2, 2);
    	this->cumulativeRenderPlane.setPosition({pngRenderer->resolutionX/2, pngRenderer->resolutionY/2, 0.0f});

        for (unsigned int i = 0; i < this->passes.size(); i++) {
            this->passes[i]->UpdateResolution(this->pngRenderer->resolutionX, this->pngRenderer->resolutionY);
        }
    }
}

void ShaderChain::BeginSaveFrames() {

    if (fft.currentState == InputStateSoundFile) {
        this->time = 0.0;
        ofFloatColor *black = new ofFloatColor(0.0, 0.0, 0.0, 0.0);
        for (unsigned int i = 0; i < this->passes.size(); i++) {
            if (passes[i]->lastBuffer.isAllocated()) {
                passes[i]->lastBuffer.clearColorBuffer(*black);
            }
        }
        delete black;
    }

    pngRenderer->Start();
}

void ShaderChain::Update() {

    bool capturingThisFrame = pngRenderer->isCapturing;

    ofClear(25);
    if (this->passes.size() > 0) {
        UpdateResolutionIfChanged(false);
        UpdateCamera();
        fft.Update();

        ofFloatColor black;
        black.a = 1;

        if (capturingThisFrame) {
            pngRenderer->Tick();
        }
        float deltaTime = 1. / (pngRenderer->FPS * pngRenderer->numBlendFrames);

        if (this->isRunning) {
            this->cumulativeBuffer.begin();
            ofClear(0,0,0,255);
            this->cumulativeBuffer.end();

            this->cumulativeBufferSwap.begin();
            ofClear(0,0,0,255);
            this->cumulativeBufferSwap.end();

            for (unsigned int i = 0; i < pngRenderer->numBlendFrames; i++) {

                if (capturingThisFrame) {
                    this->time = this->time + deltaTime;
                    fft.setTime(this->time);
                }
                else {
                    this->time = pngRenderer->preview ? fmod(this->time + deltaTime, pngRenderer->animduration) : this->time + deltaTime;
				}

                if (frame % pngRenderer->frameskip == 0) {
                    RenderPasses();
                }

                this->cumulativeBuffer.begin();
                this->cumulativeShader.begin();
                ofClear(0, 0, 0, 255);
                int idx = this->passes.size()-1;
                this->cumulativeShader.setUniform1f("factor", (1./pngRenderer->numBlendFrames));
                this->cumulativeShader.setUniformTexture("_CumulativeTexture", this->cumulativeBufferSwap.getTexture(), 0);
                this->cumulativeShader.setUniformTexture("_IncomingTexture", this->passes[idx]->buffer.getTexture(), 1);
                this->cumulativeRenderPlane.draw();
                this->cumulativeShader.end();
                this->cumulativeBuffer.end();

                auto swap = this->cumulativeBuffer;
                this->cumulativeBuffer = this->cumulativeBufferSwap;
                this->cumulativeBufferSwap = swap;
            }
        }
    }
    if (this->passes.size() > 0) {
        int idx = this->passes.size()-1;
        float x = ofGetWidth()/2.-this->pngRenderer->resolutionX*0.5*this->pngRenderer->displayScaleParam;
        float y = ofGetHeight()/2.-this->pngRenderer->resolutionY*0.5*this->pngRenderer->displayScaleParam;
        float w = this->pngRenderer->resolutionX*this->pngRenderer->displayScaleParam;
        float h = this->pngRenderer->resolutionY*this->pngRenderer->displayScaleParam;

        this->cumulativeBufferSwap.draw(x, y, w, h);
    }

    if (capturingThisFrame) {
        this->pngRenderer->WritePNG(&(this->cumulativeBufferSwap));
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
        guiGlobal->setEnabled(this->showGui);
        parameterPanel->setEnabled(this->showGui);
        passesGui->panel->setEnabled(this->showGui);
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
    if (key == 'r') {
        this->time = 0.0;
        fft.resetSongIfPlaying();
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
    if (isMouseDown && !pngRenderer->isCapturing) {
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
		UpdateResolutionIfChanged(true);
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
    UpdateResolutionIfChanged(true);
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
        outputMp4Filename = outputMp4AudioFilename;
    }

    if (pngRenderer->numLoops > 1) {
        string inputFileText = "";
        for (unsigned int i = 0; i < pngRenderer->numLoops; i++) {
            inputFileText += "file '" + outputMp4Filename + "''\n";
        }

        ofstream file;
        file.open("list.txt");
        file << inputFileText;
        file.close();

        string outputLoopedFilename = outputFilename + "_looped.mp4";

        ffmpegCommand = "ffmpeg -f concat -i list.txt -c copy " + outputLoopedFilename;
        system(ffmpegCommand.c_str());

        remove("list.txt");
        outputMp4Filename = outputLoopedFilename;
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
    int totalFrames = pngRenderer->FPS * pngRenderer->animduration;
    string totalZerosString = to_string((int)floor(log10 (((float)totalFrames)))+1);

    string s = pngRenderer->presetNameParam.get();
    string file = s.substr(s.find_last_of("/") + 1);
    string fileWithoutExtension = file.substr(0, file.find_last_of("."));
    string rendersDirectory = "data/renders/";
    string targetDirectory = rendersDirectory + fileWithoutExtension + "_gif/";
    system(("mkdir " + targetDirectory).c_str());
    string moveFilesCommand = "mv " + rendersDirectory + fileWithoutExtension + "_*.png " + targetDirectory;
    system(moveFilesCommand.c_str());

    string ffmpegCommand = "ffmpeg -v warning -start_number 0 -i " + targetDirectory + fileWithoutExtension + "_%0"+totalZerosString+"d.png -vf scale=500:-1:flags=lanczos,palettegen=stats_mode=diff:reserve_transparent=off:max_colors="+to_string(pngRenderer->gifNumColors)+ " -y " + targetDirectory + "palette.png";
    system(ffmpegCommand.c_str());

    string targetFilename = targetDirectory + fileWithoutExtension +".gif";
    ffmpegCommand = "ffmpeg -v warning -thread_queue_size 512 -start_number 0 -i " + targetDirectory + fileWithoutExtension + "_%0" + totalZerosString+"d.png -i "+ targetDirectory + "palette.png -r 30 -lavfi scale=500:-1:flags=\"lanczos [x]; [x][1:v] paletteuse\" -y " + targetFilename;
    system(ffmpegCommand.c_str());

    updateStatusText("Gif saved to " + targetFilename);

}
