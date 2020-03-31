#include "ShaderPass.h"

#include <regex>

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
    parameterGroup.setName(shaderPath);
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
    this->params[params.size()-1]->AddToGui(&parameterGroup);
}

void ShaderPass::AddVector3Parameter(std::string s, glm::vec3 val, bool show, glm::vec2 range, int midi[]) {
    auto ptr = std::make_unique<Vector3Parameter>(s, val, show, range, midi);
    this->params.push_back(std::move(ptr));
    this->params[params.size()-1]->AddToGui(&parameterGroup);
}

void ShaderPass::Render(float time, ofNode *cam, FFTManager *fft) {
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
        this->shader.setUniformTexture("_AudioTexture", fft->audioTexture, 2);
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

void ShaderPass::LoadFromJson(Json::Value &json, float width, float height) {

    std::string shaderName = json["shaderName"].asString();
    Load(shaderName, glm::vec2(width, height));
    this->wantsLastBuffer = json["wantsLastBuffer"].asBool();

    for (int j = 0; j < json["parameters"].size(); j++)
    {
        int type = json["parameters"][j]["type"].asInt();

        // floats
        if (type == 0) {
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
        else if (type == 1) {
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
    }
}

// Function by Will Gallia https://github.com/whg/ofxUniformGui/blob/master/src/ofxUniformGui.cpp
void ShaderPass::ParseUniforms(string path) {

    ofFile file(path);

    cout << "Parsing uniforms for " << this->filePath << " " << file.isFile() << endl;
    static string tokens[] = { "line", "type", "name", "default", "min", "max" };
    static int NTOKENS = 6;

    std::regex rgx("^\\s*uniform");

    string line;

    while(std::getline(file, line)) {
        std::smatch smatch;

        if (std::regex_search(line, smatch, rgx)) {
            stringstream s(line);
            string word;
            vector<string> words;

            while (s >> word)
            {
                words.push_back(word);
            }

            if (words.size() != 3) {
                continue;
            }

            string type = words[1];
            string name = words[2];
            name.pop_back();
            
            if (type == "float") {
                AddFloatParameter(name, 0.5, glm::vec2(0.0, 1.0), true, -1);
            }
            else if (type == "vec3") {
                int midi[] = {-1, -1, -1};
                AddVector3Parameter(name, glm::vec3(0.0, 0.0, 0.0), true, glm::vec2(0.0, 1.0), midi);
            }
        }

        /*
        if (matches.size() >= NTOKENS) {

            map<string, string> values;
            for (int i = 0; i < NTOKENS; i++) {
                values[tokens[i]] = line.substr(matches[i].offset, matches[i].length);
            }

            float min = atof(values["min"].c_str());
            float max = atof(values["max"].c_str());

            if (values["type"].find("vec") == string::npos) {

                // primitive types

                if (values["type"] == "int") {
                    int def = atoi(values["default"].c_str());
                    //add(shader, values["name"], def, min, max);
                }
                else if (values["type"] == "float") {
                    float def = atof(values["default"].c_str());
                    AddFloatParameter(values["name"], 0.5, glm::vec2(0.0,1.0), true, -1);
                }
            }
            else {
                if (values["type"] == "vec2") {
                    RegularExpression vec2Re("vec2\\(([0-9.]+)\\s*,\\s*([0-9.]+)\\)");
                    vec2Re.match(values["default"], 0, matches);

                    if (matches.size() >= 3) {
                        string vx = values["default"].substr(matches[1].offset, matches[1].length);
                        string vy = values["default"].substr(matches[2].offset, matches[2].length);
                        ofVec2f def(atof(vx.c_str()), atof(vy.c_str()));

                        //add(shader, values["name"], def, min, max);
                    }
                    else {
                        ofLogError() << "can't get default values of vec2 " << values["name"];
                    }

                }
                else if (values["type"] == "vec3") {
                    RegularExpression vec2Re("vec3\\(([0-9.]+)\\s*,\\s*([0-9.]+),\\s*([0-9.]+)\\)");
                    vec2Re.match(values["default"], 0, matches);

                    if (matches.size() >= 4) {
                        string vx = values["default"].substr(matches[1].offset, matches[1].length);
                        string vy = values["default"].substr(matches[2].offset, matches[2].length);
                        string vz = values["default"].substr(matches[3].offset, matches[3].length);
                        ofVec3f def(atof(vx.c_str()), atof(vy.c_str()), atof(vz.c_str()));
                        int midi[] = {-1, -1, -1};

                        AddVector3Parameter(values["name"], glm::vec3(vx, vy, xz), true, glm::vec2(0.0, 1.0), midi);
                    //    add(shader, values["name"], def, min, max);
                    }
                    else {
                        ofLogError() << "can't get default values of vec3 " << values["name"];
                    }

                }
            }

        }
*/
    }
}
