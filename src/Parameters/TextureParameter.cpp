#include "TextureParameter.h"
#include "ShaderPass.h"

TextureParameter::TextureParameter(string uniform, string filePath, int textureIndex, bool show, string texType, string targetBufferName) {
    this->textureIndex = textureIndex;
    this->filePath = filePath;
    this->uniform = uniform;
  //  auto player = ofPtr<ofBaseVideoPlayer>(&gstreamer);
  //  this->videoFile.setPlayer(player);
    this->show = show;
    this->type = getTypeFromString(texType);
    this->targetBufferName = targetBufferName;
}

void TextureParameter::close() {
    closeVideoFile();
}

void TextureParameter::update(RenderStruct *renderStruct) {
    if (this->type == VideoFile) {
        if (renderStruct->isOfflineRendering) {
            this->videoFile.update();
            this->videoFile.nextFrame();

        } else {
            if (this->videoFile.isPlaying()) {
                this->videoFile.update();
            }
        }
    }
}

void TextureParameter::updateToNewType(TextureSourceType t) {
    if (this->type == Webcam && t != Webcam) {
        texInput->selectionView->updateWebcam(false);
    }
    if (this->type == VideoFile) {
        closeVideoFile();
    }
    this->type = t;
}

void TextureParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {

    if (this->type == ImageFile) {
        if (this->value.isAllocated()) {
            shader->setUniformTexture(this->uniform, this->value.getTexture(), this->textureIndex);
            shader->setUniform2f(this->uniform+"_res", this->value.getWidth(), this->value.getHeight());
            this->texInput->setGraphics(&this->value.getTexture());
        }
    } else if (this->type == VideoFile) {
        if (this->videoFile.isLoaded() && this->videoFile.getWidth() > 0) {
            this->texInput->setGraphics(&this->videoFile.getTexture());
            shader->setUniformTexture(this->uniform, this->videoFile.getTexture(), 1);
            shader->setUniform2f(this->uniform+"_res", this->videoFile.getWidth(), this->videoFile.getHeight());
        }
    } else if (this->type == Webcam) {
        renderStruct->vidGrabber->update();
        this->texInput->setGraphics(&renderStruct->vidGrabber->getTexture());
        shader->setUniformTexture(this->uniform, renderStruct->vidGrabber->getTexture(), this->textureIndex);
        shader->setUniform2f(this->uniform+"_res", renderStruct->vidGrabber->getWidth(), renderStruct->vidGrabber->getHeight());
    } else if (this->type == Buffer || this->type == Last) {

        int targetBufferIndex = -1;

        for (unsigned int i = 0; i < renderStruct->passes->size(); i++) {
            if (renderStruct->passes->at(i)->displayName == targetBufferName) {
                targetBufferIndex = i;
                break;
            }
        }

        if (targetBufferIndex != -1) {

            if (this->type == Buffer) {
                this->texInput->setGraphics(&renderStruct->passes->at(targetBufferIndex)->buffer.getTexture());
                shader->setUniformTexture(this->uniform, renderStruct->passes->at(targetBufferIndex)->buffer.getTexture(), this->textureIndex);
                shader->setUniform2f(this->uniform+"_res", renderStruct->passes->at(targetBufferIndex)->buffer.getWidth(), renderStruct->passes->at(targetBufferIndex)->buffer.getHeight());
            }
        }
        if (this->type == Last) {
            if (renderStruct->lastBuffer->isAllocated()) {
                this->texInput->setGraphics(&renderStruct->lastBuffer->getTexture());
                shader->setUniformTexture(this->uniform, renderStruct->lastBuffer->getTexture(), textureIndex);
                shader->setUniformTexture(this->uniform, renderStruct->lastBuffer->getTexture(), this->textureIndex);
                shader->setUniform2f(this->uniform+"_res", renderStruct->lastBuffer->getWidth(), renderStruct->lastBuffer->getHeight());
            }
        }
    } else if (type == Audio) {
        if (renderStruct->fft->audioTexture.isAllocated()) {
            this->texInput->setGraphics(&renderStruct->fft->audioTexture);
            shader->setUniformTexture(this->uniform, renderStruct->fft->audioTexture, this->textureIndex);
            shader->setUniform2f(this->uniform+"_res", renderStruct->fft->audioTexture.getWidth(), renderStruct->fft->audioTexture.getHeight());
        }
    }
}

void TextureParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        texInput = gui->add<ofxGuiTextureInput>(this->uniform, &value.getTexture(), ofJson({{"height", 200}}));
        texInput->selectionView = selectionView;
        ofAddListener(texInput->showEvent, this, &TextureParameter::onShowSelectionView);
    }
    startDoingThingForType();
}

void TextureParameter::onShowSelectionView() {
    ofAddListener(selectionView->selectedFilePathChanged, this, &TextureParameter::updateTextureFromFile);
    ofAddListener(selectionView->wantsWebcamChanged, this, &TextureParameter::wantsWebcamChanged);
    ofAddListener(selectionView->selectedBufferChanged, this, &TextureParameter::wantsBufferChanged);
    ofAddListener(selectionView->wantsAudioChanged, this, &TextureParameter::wantsAudioChanged);

    listenersAdded = true;
}

void TextureParameter::onHideSelectionView() {
    if (listenersAdded) {
        ofRemoveListener(selectionView->selectedFilePathChanged, this, &TextureParameter::updateTextureFromFile);
        ofRemoveListener(selectionView->wantsWebcamChanged, this, &TextureParameter::wantsWebcamChanged);
        ofRemoveListener(selectionView->selectedBufferChanged, this, &TextureParameter::wantsBufferChanged);
        ofRemoveListener(selectionView->wantsAudioChanged, this, &TextureParameter::wantsAudioChanged);

        listenersAdded = false;
    }
}

void TextureParameter::updateTextureFromFile(string &s) {
    auto extension = s.substr(s.find_last_of(".") + 1);
    cout << "Loading " << s << endl;
    if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp" || extension == "gif") {
        this->value.load(s);
        updateToNewType(ImageFile);
        filePath = s;
    }

    else if (extension == "mp4" || extension == "mov" || extension == "avi" || extension == "mkv") {
        updateToNewType(VideoFile);
        this->videoFile.load(s);
        this->videoFile.setUseTexture(true);
        this->videoFile.play();
        filePath = s;
    }
    onHideSelectionView();
}

void TextureParameter::wantsWebcamChanged(bool &val) {
    if (val) {
        updateToNewType(Webcam);
    }
    onHideSelectionView();
}

void TextureParameter::wantsBufferChanged(string &val) {

    if (val == this->parentBufferName) {
        updateToNewType(Last);
    } else {
        updateToNewType(Buffer);
    }

    targetBufferName = val;
    onHideSelectionView();
}

void TextureParameter::startDoingThingForType() {
    if (type == ImageFile || type == VideoFile) {
        updateTextureFromFile(filePath);
    }
    else if (type == Webcam) {
        texInput->selectionView->updateWebcam(true);
    }
}

void TextureParameter::UpdateJson(Json::Value &val) {
    val["name"] = this->uniform;
    val["filePath"] = this->filePath;
    val["textureIndex"] = this->textureIndex;
    val["show"] = this->show;
    val["type"] = "texture";
    val["textype"] = getTextureType();
    val["targetBufferName"] = targetBufferName;
}

void TextureParameter::startOfflineRender() {
    if (type == VideoFile) {
        //this->videoFile.setPaused(true);
        //cout << "set paused" << endl;
      //  gstreamer.setFrameByFrame(true);
        this->videoFile.setPaused(true);
        this->videoFile.setFrame(0);
        this->videoFile.update();
    }
}

void TextureParameter::stopOfflineRender() {
    if (type == VideoFile) {
       // gstreamer.setFrameByFrame(false);
       //this->videoFile.setPaused(false);

        //gstreamer.setFrameByFrame(false);
        //this->videoFile.setPaused(false);
        //this->videoFile.play();
    }
}

string TextureParameter::getTextureType() {
    if (type == ImageFile) {
        return "ImageFile";
    } else if (type == VideoFile) {
        return "VideoFile";
    } else if (type == Webcam) {
        return "Webcam";
    } else if (type == Buffer) {
        return "Buffer";
    } else if (type == Last) {
        return "Last";
    } else if (type == Audio) {
        return "Audio";
    }
    return "";
}

TextureSourceType TextureParameter::getTypeFromString(string s) {
    if (s == "VideoFile") {
        return VideoFile;
    } else if (s == "Webcam") {
        return Webcam;
    } else if (s == "Buffer") {
        return Buffer;
    } else if (s == "Last") {
        return Last;
    } else if (s == "Audio") {
        return Audio;
    }
    return ImageFile;
}

bool TextureParameter::isMouseHoveredOver() {
    return texInput->isMouseOver();
}

void TextureParameter::handleInputFile(string s) {
    updateTextureFromFile(s);
}

TextureSourceType TextureParameter::getTextureSourceType() {
    return type;
}

void TextureParameter::playbackDidToggleState(bool isPaused) {
    if (type == VideoFile) {
        if (isPaused) {
            this->videoFile.setPaused(true);
        } else {
            this->videoFile.setPaused(false);
        }
    }
}

void TextureParameter::wantsAudioChanged(bool &val) {
    updateToNewType(Audio);
}

void TextureParameter::closeVideoFile() {
    if (videoFile.isLoaded() || videoFile.isPlaying()) {
        videoFile.stop();
        videoFile.close();
    }
}
