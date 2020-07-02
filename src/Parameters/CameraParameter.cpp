#include "CameraParameter.h"
#include "ofMain.h"

CameraParameter::CameraParameter(glm::vec3 pos, glm::vec3 rot) {
    this->camera.setPosition(pos);
    this->camera.setOrientation(rot);
}

CameraParameter::~CameraParameter() {
    //ofUnregisterKeyEvents(this);
}

void CameraParameter::UpdateShader(ofxAutoReloadedShader *shader, RenderStruct *renderStruct) {

    bool isMouseDown = ofGetMousePressed(0);

    if (ofGetKeyPressed('w')) {
        camera.move(-camera.getLookAtDir() * ofGetLastFrameTime());
    }
    if (ofGetKeyPressed('s')) {
        camera.move(camera.getLookAtDir() * ofGetLastFrameTime());
    }
    if (ofGetKeyPressed('d')) {
        camera.move(camera.getXAxis() * ofGetLastFrameTime());
    }
    if (ofGetKeyPressed('a')) {
        camera.move(-camera.getXAxis() * ofGetLastFrameTime());
    }

    if (isMouseDown && !renderStruct->isOfflineRendering) {
        float xDelta = ofGetMouseX() - ofGetPreviousMouseX();
        float yDelta = ofGetMouseY() - ofGetPreviousMouseY();

        camera.panDeg(xDelta * ofGetLastFrameTime() * mouseMoveSpeed);
        camera.tiltDeg(- yDelta * ofGetLastFrameTime() * mouseMoveSpeed);
    }

    shader->setUniform3f("_CamPos", camera.getPosition());
    shader->setUniform3f("_CamForward", camera.getZAxis());
    shader->setUniform3f("_CamUp", camera.getYAxis());
    shader->setUniform3f("_CamRight", camera.getXAxis());
}

void CameraParameter::AddToGui(ofxGuiGroup2 *gui) {
//    ofRegisterKeyEvents(this);
}

void CameraParameter::UpdateJson(Json::Value &val) {
    val["value"]["pos"]["x"] = camera.getX();
    val["value"]["pos"]["y"] = camera.getY();
    val["value"]["pos"]["z"] = camera.getZ();

    glm::vec3 rot = camera.getOrientationEulerDeg();
    val["value"]["rot"]["x"] = rot.x;
    val["value"]["rot"]["y"] = rot.y;
    val["value"]["rot"]["z"] = rot.z;

    val["type"] = "camera";
}
