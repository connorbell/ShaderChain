#include "TextureParameter.h"
#include "ShaderPass.h"

TextureParameter::TextureParameter(string uniform, string filePath, int textureIndex, bool show) {
    this->textureIndex = textureIndex;
    this->filePath = filePath;
    this->uniform = uniform;
    this->show = show;
    if (filePath.length() > 0) {
        this->value.load(filePath);
        this->type = ImageFile;
    }
    else {
        this->type = None;
    }
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
            this->videoFile.play();
            if (renderStruct->isOfflineRendering) {
                this->videoFile.setFrame(renderStruct->frame);
            }

            this->videoFile.update();
            this->videoFile.unbind();
            //this->texInput->setGraphics(&this->videoFile.getTexture());
            //shader->setUniformTexture(this->uniform, this->videoFile.getTexture(), this->textureIndex);
            //shader->setUniform2f(this->uniform+"_res", this->videoFile.getWidth(), this->videoFile.getHeight());
            this->videoFile.setPaused(true);
        }
    } else if (this->type == Webcam) {
        renderStruct->vidGrabber->update();
        this->texInput->setGraphics(&renderStruct->vidGrabber->getTexture());
        shader->setUniformTexture(this->uniform, renderStruct->vidGrabber->getTexture(), this->textureIndex);
        shader->setUniform2f(this->uniform+"_res", renderStruct->vidGrabber->getWidth(), renderStruct->vidGrabber->getHeight());
    } else if (this->type == Buffer) {

        int targetBufferIndex = -1;

        for (unsigned int i = 0; i < renderStruct->passes->size(); i++) {
            if (renderStruct->passes->at(i)->displayName == bufferName) {
                targetBufferIndex = i;
                break;
            }
        }

        if (targetBufferIndex != -1) {
            this->texInput->setGraphics(&renderStruct->passes->at(targetBufferIndex)->buffer.getTexture());
            shader->setUniformTexture(this->uniform, renderStruct->passes->at(targetBufferIndex)->buffer.getTexture(), this->textureIndex);
            shader->setUniform2f(this->uniform+"_res", renderStruct->passes->at(targetBufferIndex)->buffer.getWidth(), renderStruct->passes->at(targetBufferIndex)->buffer.getHeight());
        }
    }
}

void TextureParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        texInput = gui->add<ofxGuiTextureInput>(this->uniform, &value.getTexture(), ofJson({{"height", 200}}));
        texInput->selectionView = selectionView;
        ofAddListener(texInput->showEvent, this, &TextureParameter::onShowSelectionView);
    }
}

void TextureParameter::onShowSelectionView() {
    cout << "On show selection view" <<endl;
    ofAddListener(selectionView->selectedFilePathChanged, this, &TextureParameter::updateTextureFromFile);
    ofAddListener(selectionView->wantsWebcamChanged, this, &TextureParameter::wantsWebcamChanged);
    ofAddListener(selectionView->selectedBufferChanged, this, &TextureParameter::wantsBufferChanged);
}

void TextureParameter::onHideSelectionView() {
    cout << "On hide selection view" <<endl;
    ofRemoveListener(selectionView->selectedFilePathChanged, this, &TextureParameter::updateTextureFromFile);
    ofRemoveListener(selectionView->wantsWebcamChanged, this, &TextureParameter::wantsWebcamChanged);
    ofRemoveListener(selectionView->selectedBufferChanged, this, &TextureParameter::wantsBufferChanged);
}

void TextureParameter::updateTextureFromFile(string &s) {
    auto extension = s.substr(s.find_last_of(".") + 1);
    cout << "Loading " << s << endl;
    if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "bmp" || extension == "gif") {
        this->value.load(s);
        this->type = ImageFile;
    }

    if (extension == "mp4") {
        this->type = VideoFile;
        this->videoFile.load(s);
        this->videoFile.setUseTexture(true);
        this->videoFile.play();
    //    this->texInput->setGraphics(&this->videoFile.getTexture());
    }
    onHideSelectionView();
}

void TextureParameter::wantsWebcamChanged(bool &val) {
    this->type = Webcam;
    cout << "lets go webcam" << endl;
    onHideSelectionView();
}

void TextureParameter::wantsBufferChanged(string &val) {
    this->type = Buffer;
    bufferName = val;
    cout << uniform << " wants " << bufferName << endl;
    onHideSelectionView();
}

void TextureParameter::UpdateJson(Json::Value &val) {
    val["name"] = this->uniform;
    val["filePath"] = this->filePath;
    val["textureIndex"] = this->textureIndex;
    val["show"] = this->show;
    val["type"] = 2;
}
