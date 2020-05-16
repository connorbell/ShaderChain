#include "TextureParameter.h"

TextureParameter::TextureParameter(string uniform, string filePath, int textureIndex, bool show) {
    this->textureIndex = textureIndex;
    this->filePath = filePath;
    this->uniform = uniform;
    this->show = show;
    if (filePath.length() > 0) {
        this->value.load(filePath);
    }
}

void TextureParameter::UpdateShader(ofxAutoReloadedShader *shader) {
    shader->setUniformTexture(this->uniform, this->value, this->textureIndex);
    shader->setUniform2f(this->uniform+"_res", this->value.getWidth(), this->value.getHeight());
}

void TextureParameter::AddToGui(ofxGuiGroup2 *gui) {
    if (this->show) {
        gui->add<ofxGuiGraphics>(this->uniform, &value.getTexture(), ofJson({{"height", 200}}));
    }
}

void TextureParameter::UpdateJson(Json::Value &val) {
    val["name"] = this->uniform;
    val["filePath"] = this->filePath;
    val["textureIndex"] = this->textureIndex;
    val["show"] = this->show;
    val["type"] = 2;
}
