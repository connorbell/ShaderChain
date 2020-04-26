#pragma once

#include <map>
#include <vector>
#include <string>
#include "ofMain.h"

#include "ofxAutoReloadedShader.h"
#include "ofxJSON.h"

#include "Parameters/Parameter.h"
#include "Parameters/FloatParameter.h"
#include "Parameters/Vector3Parameter.h"
#include "Parameters/TextureParameter.h"
#include "Parameters/ColorParameter.h"

#include "ofxAutoReloadedShader.h"
#include "FFTManager.h"

class ShaderPass {
  public:
    std::string filePath;
    std::string displayName;
    std::vector<std::unique_ptr<Parameter>> params;
    glm::vec2 targetResolution;
    float scale;
    ofxAutoReloadedShader shader;
    ofxGuiGroup2 *parameterGroup;
    ofFbo buffer;
    ofFbo lastBuffer;
    ofPlanePrimitive plane;
    bool wantsLastBuffer;
    bool wantsCamera;

    ShaderPass();
    ~ShaderPass();
    ShaderPass(std::string shaderPath, glm::vec2 res);

    void Load(std::string shaderPath, glm::vec2 res);
    void LoadFromJson(Json::Value &json, float width, float height);

    void AddFloatParameter(std::string s, float startValue, glm::vec2 range, bool show, int midi);
    void AddVector3Parameter(std::string s, glm::vec3 val, bool show, glm::vec2 range, int midi[]);
    void AddTextureParameter(string s, string filePath, int textureIndex, bool show);
    void AddColorParameter(string s, float r, float g, float b, float a, bool show, int midi[]);
    void Render(float time, ofNode *cam, FFTManager *fft);
    void SetInputTexture(ofFbo buffer);
    void UpdateTime(float time);
    void UpdateResolution(int x, int y);
    void LoadJsonParametersFromLoadedShader();
    void AddToGui(ofxGuiPanel *gui);
    void LoadDisplayNameFromFileName();

  private:
    ofxJSON json;
    void LoadParametersFromJson(Json::Value &json);
};
