#pragma once

#include <map>
#include <vector>
#include <string>
#include "ofMain.h"
#include "Parameters/Parameter.h"
#include "Parameters/FloatParameter.h"
#include "Parameters/Vector3Parameter.h"
#include "ofxAutoReloadedShader.h"

class ShaderPass {
  public:
    std::string filePath;
    std::vector<std::unique_ptr<Parameter>> params;
    float scale;
    ofxAutoReloadedShader shader;
    ofFbo buffer;
    ofFbo lastBuffer;
    ofPlanePrimitive plane;
    bool wantsLastBuffer;

    ShaderPass();
    ~ShaderPass();
    ShaderPass(std::string shaderPath, glm::vec2 res);

    void Load(std::string shaderPath, glm::vec2 res);
    void AddFloatParameter(std::string s, float startValue, glm::vec2 range, bool show, int midi);
    void AddVector3Parameter(std::string s, glm::vec3 val, bool show, glm::vec2 range, int midi[]);
    void Render(float time, ofNode *cam);
    void SetInputTexture(ofFbo buffer);
    void UpdateTime(float time);
    void UpdateResolution(int x, int y);

  private:
    glm::vec2 targetResolution;
};
