#include "ShaderPass.h"

ShaderPass::ShaderPass() {

}
ShaderPass::ShaderPass(std::string shaderPath, glm::vec2 res) {
    Load(shaderPath, res);
}
void ShaderPass::Load(std::string shaderPath, glm::vec2 res) {
    this->shader.load(shaderPath);
    this->filePath = shaderPath;
    this->scale = 1.0;
    this->targetResolution = glm::vec2(res.x * scale, res.y * scale);
    cout << filePath << " Target Resolution: " << std::to_string(this->targetResolution.x) << " " << std::to_string(this->targetResolution.y) << endl;
    UpdateResolution(this->targetResolution.x, this->targetResolution.y);
}

ShaderPass::~ShaderPass(){
    params.clear();
}

void ShaderPass::UpdateResolution(int x, int y) {
    cout << "update resolution" << endl;
    this->targetResolution = glm::vec2(x * scale, y * scale);
    this->buffer.allocate(targetResolution.x, targetResolution.y);
    this->lastBuffer.allocate(targetResolution.x, targetResolution.y);
    this->plane.set(this->targetResolution.x, this->targetResolution.y, 2, 2);
	this->plane.setPosition({this->targetResolution.x/2, this->targetResolution.y/2, 0.0f});
}

void ShaderPass::AddFloatParameter(std::string s, float startValue, glm::vec2 range, bool show, int midi) {
    auto ptr = std::make_unique<FloatParameter>(s, startValue, range, show, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddVector3Parameter(std::string s, glm::vec3 val, bool show, glm::vec2 range, int midi[]) {
    auto ptr = std::make_unique<Vector3Parameter>(s, val, show, range, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::Render(float time, ofNode *cam) {
    this->buffer.begin();
    this->shader.begin();

    UpdateTime(time);
    this->shader.setUniform2f("_Resolution", this->targetResolution.x, this->targetResolution.y);
    this->shader.setUniform3f("_CamPos", cam->getPosition());
    this->shader.setUniform3f("_CamForward", cam->getZAxis());
    this->shader.setUniform3f("_CamUp", cam->getYAxis());
    this->shader.setUniform3f("_CamRight", cam->getXAxis());

    if (this->wantsLastBuffer) {
        this->shader.setUniformTexture("_LastTexture", this->lastBuffer.getTexture(), 1);
    }

    for (uint i = 0; i < params.size(); i++) {
        this->params[i]->UpdateShader(&(this->shader));
    }

    this->plane.draw();

    this->shader.end();
    this->buffer.end();

    if (this->wantsLastBuffer) {
        this->lastBuffer.begin();
        this->buffer.draw(0,0);
        this->lastBuffer.end();
    }
}

void ShaderPass::SetInputTexture(ofFbo buffer) {
  this->shader.setUniformTexture("_MainTexture", buffer.getTexture(), 0);
}

void ShaderPass::UpdateTime(float time) {
    this->shader.setUniform1f("_Time", time);
}
