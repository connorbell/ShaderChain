#include "TextureParameter.h"

void TextureParameter::UpdateShader(ofxAutoReloadedShader *shader) {
    shader->setUniformTexture(this->uniform, this->value, 0);
}
