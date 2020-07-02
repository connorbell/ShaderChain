#pragma once
#include <string>

#include "Parameter.h"

class CameraParameter : public Parameter {

public:

    CameraParameter(glm::vec3 pos, glm::vec3 rot);
    ~CameraParameter();
    ofParameter<ofVec3f> position;
    ofVec3f left;
    ofVec3f right;
    ofVec3f up;
    virtual void UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) override;
    virtual void AddToGui(ofxGuiGroup2 *gui) override;
    virtual void UpdateJson(Json::Value &val) override;


    void keyPressed(int key);
    void keyReleased(int key);

private:
    ofNode camera;
    float mouseMoveSpeed = 6.0;


};
