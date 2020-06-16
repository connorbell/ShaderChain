#pragma once

#include <map>
#include <vector>
#include <string>
#include "ofMain.h"

#include "ofxAutoReloadedShader.h"
#include "ofxJSON.h"

#include "Parameters/Parameter.h"
#include "Parameters/FloatParameter.h"
#include "Parameters/Vector2Parameter.h"
#include "Parameters/Vector3Parameter.h"
#include "Parameters/Vector4Parameter.h"
#include "Parameters/TextureParameter.h"
#include "Parameters/ColorParameter.h"
#include "Parameters/BoolParameter.h"
#include "Parameters/IntParameter.h"
#include "Parameters/CameraParameter.h"

#include "ofxAutoReloadedShader.h"
#include "FFTManager.h"
#include "RenderStruct.h"

class ShaderPass {
  public:
    std::string filePath;
    std::string vertexShaderPath = "shaders/internal/vertex.vert";
    std::string displayName;
    std::vector<std::unique_ptr<Parameter>> params;
    glm::vec2 targetResolution;
    float scale;
    ofxAutoReloadedShader shader;
    ofxGuiGroup2 *parameterGroup;
    ofFbo buffer;
    ofFbo lastBuffer;
    ofPlanePrimitive plane;
    bool wantsLastBuffer = false;
    ShaderPass();
    ~ShaderPass();
    ShaderPass(std::string shaderPath, glm::vec2 res);

    void Load(std::string shaderPath, glm::vec2 res);
    void LoadFromJson(Json::Value &json, float width, float height);

    void AddFloatParameter(std::string s, float startValue, glm::vec2 range, bool show, int midi);
    void AddIntParameter(std::string s, int startValue, glm::vec2 range, bool show, int midi);
    void AddBoolParameter(std::string s, bool startValue, bool show, int midi);
    void AddVector2Parameter(std::string s, glm::vec2 val, bool show, glm::vec2 range, int midi[]);
    void AddVector3Parameter(std::string s, glm::vec3 val, bool show, glm::vec2 range, int midi[]);
    void AddVector4Parameter(std::string s, glm::vec4 val, bool show, glm::vec2 range, int midi[]);
    void AddTextureParameter(string s, string filePath, int textureIndex, bool show, string texType, string targetBufferName);
    void AddColorParameter(string s, float r, float g, float b, float a, bool show, int midi[]);
    void addCameraParameter(glm::vec3 pos, glm::vec3 rot);
    void Render(ofFbo *previousBuffer, RenderStruct *renderStruct);
    void SetInputTexture(ofFbo *buffer);
    void UpdateTime(float time);
    void UpdateResolution(int x, int y);
    void LoadJsonParametersFromLoadedShader();
    void AddToGui(ofxGuiContainer *gui, TextureInputSelectionView *selectionView);
    void LoadDisplayNameFromFileName();
    void update(RenderStruct *renderStruct);
    void startOfflineRender();
    void stopOfflineRender();
    void updateShaderJson();
  private:
    ofxJSON json;
    void LoadParametersFromJson(Json::Value &json);
};
