#include "TextureParameter.h"
#include "ShaderPass.h"

TextureParameter::TextureParameter(string uniform, string filePath, int textureIndex, bool show, string texType, string targetBufferName) {
    this->textureIndex = textureIndex;
    this->filePath = filePath;
    this->uniform = uniform;
    gstreamer.setFrameByFrame(true);
    auto player = ofPtr<ofBaseVideoPlayer>(&gstreamer);
    this->videoFile.setPlayer(player);
    this->show = show;
    this->type = getTypeFromString(texType);
    this->targetBufferName = targetBufferName;
}

void TextureParameter::update(RenderStruct *renderStruct) {
    if (this->type == VideoFile) {

        if (renderStruct->isOfflineRendering) {
            this->videoFile.nextFrame();
            this->videoFile.update();
        } else {
        //    this->videoFile.update();
        }
    }
}

void TextureParameter::updateToNewType(TextureSourceType t) {
    if (this->type == Webcam && t != Webcam) {
        texInput->selectionView->updateWebcam(false);
    }
    if (this->type == VideoFile) {
        cout << "unLoading video file" << endl;
        this->texInput->setGraphics(&this->value.getTexture());

        if (videoFile.isLoaded()) {
            cout << "video is loaded" << endl;

            videoFile.stop();
            cout << "stopped" << endl;

            videoFile.close();
            cout << "closed" << endl;


        }

        cout << "unLoaded video file" << endl;

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
            if (this->type == Last) {
                if (renderStruct->lastBuffer->isAllocated()) {
                    this->texInput->setGraphics(&renderStruct->lastBuffer->getTexture());
                    shader->setUniformTexture(this->uniform, renderStruct->lastBuffer->getTexture(), targetBufferIndex);
                    shader->setUniformTexture(this->uniform, renderStruct->lastBuffer->getTexture(), this->textureIndex);
                    shader->setUniform2f(this->uniform+"_res", renderStruct->lastBuffer->getWidth(), renderStruct->lastBuffer->getHeight());
                }
            }
        }
    }
}

void TextureParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        cout << "adding to gui " << endl;
        texInput = gui->add<ofxGuiTextureInput>(this->uniform, &value.getTexture(), ofJson({{"height", 200}}));
        cout << "added to gui" << endl;
        texInput->selectionView = selectionView;
        ofAddListener(texInput->showEvent, this, &TextureParameter::onShowSelectionView);
    }
    startDoingThingForType();
}

void TextureParameter::onShowSelectionView() {
    ofAddListener(selectionView->selectedFilePathChanged, this, &TextureParameter::updateTextureFromFile);
    ofAddListener(selectionView->wantsWebcamChanged, this, &TextureParameter::wantsWebcamChanged);
    ofAddListener(selectionView->selectedBufferChanged, this, &TextureParameter::wantsBufferChanged);
    listenersAdded = true;
}

void TextureParameter::onHideSelectionView() {
    if (listenersAdded) {
        ofRemoveListener(selectionView->selectedFilePathChanged, this, &TextureParameter::updateTextureFromFile);
        ofRemoveListener(selectionView->wantsWebcamChanged, this, &TextureParameter::wantsWebcamChanged);
        ofRemoveListener(selectionView->selectedBufferChanged, this, &TextureParameter::wantsBufferChanged);
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
        cout << "Loading video file" << endl;
        this->videoFile.load(s);
        this->videoFile.setUseTexture(true);
        this->videoFile.play();
        filePath = s;
    }
    onHideSelectionView();
}

void TextureParameter::wantsWebcamChanged(bool &val) {
    updateToNewType(Webcam);
    cout << "lets go webcam" << endl;
    onHideSelectionView();
}

void TextureParameter::wantsBufferChanged(string &val) {

    if (val == this->parentBufferName) {
        updateToNewType(Last);
    } else {
        updateToNewType(Buffer);
    }

    targetBufferName = val;
    cout << uniform << " wants " << targetBufferName << endl;
    onHideSelectionView();
}

void TextureParameter::startDoingThingForType() {
    cout << getTextureType() << endl;
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
        cout << "start offline render" << endl;
        this->videoFile.setPaused(true);
        this->videoFile.setFrame(0);
        gstreamer.setFrameByFrame(true);
        //gstreamer.setFrameByFrame(true);
    }
}

void TextureParameter::stopOfflineRender() {
    if (type == VideoFile) {
        this->videoFile.setPaused(false);
        this->videoFile.play();

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
        this->videoFile.setPaused(isPaused);
        cout << "Toggle paused " << isPaused << endl;
        if (!isPaused) {
            this->videoFile.play();
        }
    }
}
