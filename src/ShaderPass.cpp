#include "ShaderPass.h"
#include "ShaderChain.h"

#include <regex>

ShaderPass::ShaderPass() {

}

ShaderPass::ShaderPass(std::string shaderPath, glm::vec2 res) {
    Load(shaderPath, res);
}

void ShaderPass::Load(std::string shaderPath, glm::vec2 res) {
    this->shader.load(vertexShaderPath, shaderPath + ".frag", "");
    this->filePath = shaderPath;
    this->scale = 1.0;
    this->targetResolution = glm::vec2(res.x * scale, res.y * scale);
    this->parameterGroup = nullptr;
    this->LoadDisplayNameFromFileName();
    cout << filePath << " Target Resolution: " << std::to_string(this->targetResolution.x) << " " << std::to_string(this->targetResolution.y) << endl;
    UpdateResolution(this->targetResolution.x, this->targetResolution.y);
}

ShaderPass::~ShaderPass(){
    params.clear();
}

void ShaderPass::LoadDisplayNameFromFileName() {
    int indexOfLastForwardSlash = this->filePath.find_last_of("/");
    int indexOfLastBackSlash = this->filePath.find_last_of("\\");

    if (indexOfLastForwardSlash != std::string::npos) {
        this->displayName = this->filePath.substr(indexOfLastForwardSlash + 1);
    } else if (indexOfLastBackSlash != std::string::npos) {
        this->displayName = this->filePath.substr(indexOfLastBackSlash + 1);
    } else {
        this->displayName = this->filePath;
    }
}

void ShaderPass::UpdateResolution(int x, int y) {
    cout << "update resolution" << endl;
    this->targetResolution = glm::vec2(x * scale, y * scale);
    this->buffer.allocate(targetResolution.x, targetResolution.y, GL_RGBA32F);

    if (lastBufferTextureIndex != -1) {
        this->lastBuffer.allocate(targetResolution.x, targetResolution.y, GL_RGBA32F);
    }
    this->plane.set(this->targetResolution.x, this->targetResolution.y, 2, 2);
	this->plane.setPosition({this->targetResolution.x/2, this->targetResolution.y/2, 0.0f});
}

void ShaderPass::AddBoolParameter(std::string s, bool startValue, bool show, int midi) {
    auto ptr = std::make_unique<BoolParameter>(s, startValue, show, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddIntParameter(std::string s, int startValue, glm::vec2 range, bool show, int midi) {
    auto ptr = std::make_unique<IntParameter>(s, startValue, range, show, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddFloatParameter(std::string s, float startValue, glm::vec2 range, bool show, int midi) {
    auto ptr = std::make_unique<FloatParameter>(s, startValue, range, show, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddVector2Parameter(std::string s, glm::vec2 val, bool show, glm::vec2 range, int midi[]) {
    auto ptr = std::make_unique<Vector2Parameter>(s, val, show, range, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddVector3Parameter(std::string s, glm::vec3 val, bool show, glm::vec2 range, int midi[]) {
    auto ptr = std::make_unique<Vector3Parameter>(s, val, show, range, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddVector4Parameter(std::string s, glm::vec4 val, bool show, glm::vec2 range, int midi[]) {
    auto ptr = std::make_unique<Vector4Parameter>(s, val, show, range, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddTextureParameter(string s, string filePath, int textureIndex, bool show, string texType) {
    auto ptr = std::make_unique<TextureParameter>(s, filePath, textureIndex, show, texType);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::AddColorParameter(string s, float r, float g, float b, float a, bool show, int midi[]) {
    auto ptr = std::make_unique<ColorParameter>(s, r, g, b, a, show, midi);
    this->params.push_back(std::move(ptr));
}

void ShaderPass::update(RenderStruct *renderStruct) {
    for (unsigned int i = 0; i < this->params.size(); i++) {
        this->params[i]->update(renderStruct);
    }
}

void ShaderPass::Render(ofFbo *previousBuffer, RenderStruct *renderStruct) {
    this->buffer.begin();
    this->shader.begin();

	UpdateTime(renderStruct->time);

	if (previousBuffer != nullptr) {
        SetInputTexture(previousBuffer);
    }
    this->shader.setUniform2f("_Resolution", this->targetResolution.x, this->targetResolution.y);
    if (wantsCamera) {
        this->shader.setUniform3f("_CamPos", renderStruct->cam->getPosition());
        this->shader.setUniform3f("_CamForward", renderStruct->cam->getZAxis());
        this->shader.setUniform3f("_CamUp", renderStruct->cam->getYAxis());
        this->shader.setUniform3f("_CamRight", renderStruct->cam->getXAxis());
    }

    if (this->lastBufferTextureIndex != -1 && this->lastBuffer.isAllocated()) {
        this->shader.setUniformTexture("_LastTexture", this->lastBuffer.getTexture(), this->lastBufferTextureIndex);
    }
    if (this->audioTextureIndex != -1) {
        this->shader.setUniformTexture("_AudioTexture", renderStruct->fft->audioTexture, this->audioTextureIndex);
    }

    for (unsigned int i = 0; i < params.size(); i++) {
        this->params[i]->UpdateShader(&(this->shader), renderStruct);
    }

    this->plane.draw();
    this->shader.end();
    this->buffer.end();

    if (this->lastBufferTextureIndex != -1) {
        this->lastBuffer.begin();
        this->buffer.draw(0,0);
        this->lastBuffer.end();
    }
}

void ShaderPass::SetInputTexture(ofFbo *buffer) {
  this->shader.setUniformTexture("_MainTexture", buffer->getTexture(), 0);
}

void ShaderPass::UpdateTime(float time) {
    this->shader.setUniform1f("_Time", time);
}

void ShaderPass::startOfflineRender() {
    for (unsigned int i = 0; i < this->params.size(); i++) {
        this->params[i]->startOfflineRender();
    }
}

void ShaderPass::stopOfflineRender() {
    for (unsigned int i = 0; i < this->params.size(); i++) {
        this->params[i]->stopOfflineRender();
    }
}

void ShaderPass::LoadJsonParametersFromLoadedShader() {

    ofFile textFile(filePath + ".frag");
    ofBuffer buffer(textFile, 1024);
    std::string shaderSource = buffer.getText();

    std::size_t startJsonIndex = shaderSource.find("/*");
    std::size_t endJsonIndex = shaderSource.find("*/");

    if (startJsonIndex != std::string::npos && endJsonIndex != std::string::npos) {
        string jsonString = shaderSource.substr(startJsonIndex+2, endJsonIndex-2);
        json.parse(jsonString);
        LoadParametersFromJson(json);
    }
}

void ShaderPass::LoadParametersFromJson(Json::Value &json) {
    if (json.isMember("lastBufferTextureIndex")) this->lastBufferTextureIndex = json["lastBufferTextureIndex"].asInt();
    if (json.isMember("audioTextureIndex")) this->audioTextureIndex = json["audioTextureIndex"].asInt();
    this->wantsCamera = json["wantsCamera"].asBool();
    cout << "last texture index: " << this->lastBufferTextureIndex << endl;
    if (lastBufferTextureIndex != -1) {
        this->lastBuffer.allocate(targetResolution.x, targetResolution.y, GL_RGBA32F);
    }

    for (unsigned int j = 0; j < json["parameters"].size(); j++)
    {
        string type = json["parameters"][j]["type"].asString();

        // floats
        if (type == "float") {
            float val = json["parameters"][j]["value"].asFloat();
            std::string name = json["parameters"][j]["name"].asString();
            float x = json["parameters"][j]["range"]["x"].asFloat();
            float y = json["parameters"][j]["range"]["y"].asFloat();
            bool show = json["parameters"][j]["show"].asBool();
            int midi = -1;
            if (json["parameters"][j].isMember("midi")) {
                midi = json["parameters"][j]["midi"].asInt();
            }

            AddFloatParameter(name, val, glm::vec2(x, y), show, midi);
        }
        // vec3s
        else if (type == "vec3") {
            float valx = json["parameters"][j]["value"]["x"].asFloat();
            float valy = json["parameters"][j]["value"]["y"].asFloat();
            float valz = json["parameters"][j]["value"]["z"].asFloat();

            std::string name = json["parameters"][j]["name"].asString();
            float x = json["parameters"][j]["range"]["x"].asFloat();
            float y = json["parameters"][j]["range"]["y"].asFloat();
            bool show = json["parameters"][j]["show"].asBool();

            int midi[] = {-1, -1, -1};

            if (json["parameters"][j].isMember("midi")) {
                midi[0] = json["parameters"][j]["midi"]["x"].asInt();
                midi[1] = json["parameters"][j]["midi"]["y"].asInt();
                midi[2] = json["parameters"][j]["midi"]["z"].asInt();
            }
            AddVector3Parameter(name, glm::vec3(valx, valy, valz), show, glm::vec2(x, y), midi);
        }
        // textures
        else if (type == "texture") {
            string name = json["parameters"][j]["name"].asString();
            string path = json["parameters"][j]["filePath"].asString();
            bool show = json["parameters"][j]["show"].asBool();
            int index = json["parameters"][j]["textureIndex"].asInt();
            string texType = json["parameters"][j]["textype"].asString();

            AddTextureParameter(name, path, index, show, texType);
        }
        // colors
        else if (type == "color") {
            float r = json["parameters"][j]["value"]["r"].asFloat();
            float g = json["parameters"][j]["value"]["g"].asFloat();
            float b = json["parameters"][j]["value"]["b"].asFloat();
            float a = json["parameters"][j]["value"]["a"].asFloat();
            std::string name = json["parameters"][j]["name"].asString();
            bool show = json["parameters"][j]["show"].asBool();

            int midi[] = {-1, -1, -1, -1};

            AddColorParameter(name, r, g, b, a, show, midi);
        } // vec2s
        else if (type == "vec2") {
            float valx = json["parameters"][j]["value"]["x"].asFloat();
            float valy = json["parameters"][j]["value"]["y"].asFloat();

            std::string name = json["parameters"][j]["name"].asString();
            float x = json["parameters"][j]["range"]["x"].asFloat();
            float y = json["parameters"][j]["range"]["y"].asFloat();
            bool show = json["parameters"][j]["show"].asBool();

            int midi[] = {-1, -1};

            if (json["parameters"][j].isMember("midi")) {
                midi[0] = json["parameters"][j]["midi"]["x"].asInt();
                midi[1] = json["parameters"][j]["midi"]["y"].asInt();
            }
            AddVector2Parameter(name, glm::vec2(valx, valy), show, glm::vec2(x, y), midi);
        }
        else if (type == "vec4") {
            float valx = json["parameters"][j]["value"]["x"].asFloat();
            float valy = json["parameters"][j]["value"]["y"].asFloat();
            float valz = json["parameters"][j]["value"]["z"].asFloat();
            float valw = json["parameters"][j]["value"]["w"].asFloat();

            std::string name = json["parameters"][j]["name"].asString();
            float x = json["parameters"][j]["range"]["x"].asFloat();
            float y = json["parameters"][j]["range"]["y"].asFloat();
            bool show = json["parameters"][j]["show"].asBool();

            int midi[] = {-1, -1, -1, -1};

            if (json["parameters"][j].isMember("midi")) {
                midi[0] = json["parameters"][j]["midi"]["x"].asInt();
                midi[1] = json["parameters"][j]["midi"]["y"].asInt();
                midi[2] = json["parameters"][j]["midi"]["z"].asInt();
                midi[2] = json["parameters"][j]["midi"]["w"].asInt();
            }
            AddVector4Parameter(name, glm::vec4(valx, valy, valz, valw), show, glm::vec2(x, y), midi);
        }
        else if (type == "int") {
            int val = json["parameters"][j]["value"].asInt();

            std::string name = json["parameters"][j]["name"].asString();
            float x = json["parameters"][j]["range"]["x"].asFloat();
            float y = json["parameters"][j]["range"]["y"].asFloat();
            bool show = json["parameters"][j]["show"].asBool();

            int midi = -1;

            AddIntParameter(name, val, glm::vec2(x, y), show, midi);
        } else if (type == "bool") {
            bool val = json["parameters"][j]["value"].asBool();
            std::string name = json["parameters"][j]["name"].asString();
            bool show = json["parameters"][j]["show"].asBool();

            int midi = -1;

            AddBoolParameter(name, val, show, midi);
        }
    }
}

void ShaderPass::LoadFromJson(Json::Value &json, float width, float height) {
    std::string shaderName = json["shaderName"].asString();
    Load(shaderName, glm::vec2(width, height));
    LoadParametersFromJson(json);
}

void ShaderPass::AddToGui(ofxGuiPanel *gui, TextureInputSelectionView *selectionView) {

    parameterGroup = gui->addGroup();
    parameterGroup->setName(displayName);

    for (unsigned int i = 0; i < this->params.size(); i++) {
        params[i]->selectionView = selectionView;
        params[i]->AddToGui(parameterGroup);
    }
}
