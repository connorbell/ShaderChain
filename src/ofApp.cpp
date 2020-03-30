#include <string>

#include "ofApp.h"

ofApp::ofApp() {

}
ofApp::~ofApp() {

}

//--------------------------------------------------------------
void ofApp::setup(){
    glm::vec2 res = glm::vec2(150, 150);
    ofSetWindowShape(1920, 1080);
    this->shaderChain.Setup(res);
    this->shaderChain.ReadFromJson("comps/terrain.json");
    this->shaderChain.SetupGui();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    this->shaderChain.Update();
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo info) {
	this->shaderChain.dragEvent(info);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  this->shaderChain.KeyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    this->shaderChain.isMouseDown = true;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    this->shaderChain.isMouseDown = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}
