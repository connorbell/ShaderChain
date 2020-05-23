// Fork of ofxGuiExtended ofxGuiTextureInput.cpp

#include "ofxGuiTextureInput.h"
#include "ofGraphics.h"
using namespace std;

ofxGuiTextureInput::~ofxGuiTextureInput(){
	ofRemoveListener(resize, this, &ofxGuiTextureInput::onResize);
}

ofxGuiTextureInput::ofxGuiTextureInput(std::string canvasName, TextureInputSelectionView *selectionView, const ofJson& config) {
	_bLoaded = false;
	graphics = nullptr;
	setup(canvasName);
	_setConfig(config);
    this->selectionView = selectionView;
}

ofxGuiTextureInput::ofxGuiTextureInput(std::string canvasName, ofBaseDraws * graphics, const ofJson& config)
	:ofxGuiElement(){
	_bLoaded = false;
	setGraphics(graphics);
	setup(canvasName);
	_setConfig(config);
}

ofxGuiTextureInput::ofxGuiTextureInput(std::string canvasName, ofBaseDraws * graphics, float w, float h){
	_bLoaded = false;
	setGraphics(graphics);
	setup(canvasName,w,h);
}

void ofxGuiTextureInput::setup(std::string canvasName, float w, float h){
	autoWidth = false;
	autoHeight = false;
	setName(canvasName);
	if(_bLoaded){
		if(w == 0){
			if(h == 0){
				w = getWidth();
			}else{
				w = h * graphics->getWidth() / graphics->getHeight();
			}
		}
		h = w * graphics->getHeight() / graphics->getWidth();
		ofxGuiElement::setSize(w,h);
	}
	setTheme();
	ofAddListener(resize, this, &ofxGuiTextureInput::onResize);
}

void ofxGuiTextureInput::setGraphics(ofBaseDraws *graphics){
	if(graphics){
		if(graphics->getHeight() != 0 && graphics->getWidth() != 0){
			_bLoaded = true;
			this->graphics = graphics;
		}else{
			ofLogWarning("ofxGuiTextureInput:setGraphics()", "graphics cannot be loaded, width = 0 or height = 0");
		}
	}else{
		ofLogWarning("ofxGuiTextureInput:setGraphics()", "graphics is nullptr");
	}
}

float ofxGuiTextureInput::getMinWidth(){
	float _width = 0;
	if(showName){
		_width += ofxGuiElement::getTextWidth(getName()) + 2*textPadding;
	}
	return _width;
}

float ofxGuiTextureInput::getMinHeight(){
	if(showName){
		return ofxGuiElement::getTextHeight(getName());
	}
	return 0;
}

void ofxGuiTextureInput::setAutoHeight(){
	autoHeight = true;
	autoWidth = false;
	setHeight(getWidth() * graphics->getHeight() / graphics->getWidth());
}

void ofxGuiTextureInput::setAutoWidth(){
	autoHeight = false;
	autoWidth = true;
	setWidth(getHeight() * graphics->getWidth() / graphics->getHeight());
}

void ofxGuiTextureInput::onResize(DOM::ResizeEventArgs &args){
	if(autoHeight){
		setHeight(args.shape().width * graphics->getHeight() / graphics->getWidth());
	}
	if(autoWidth){
		setWidth(args.shape().height * graphics->getWidth() / graphics->getHeight());
	}
}

void ofxGuiTextureInput::generateDraw(){
	ofxGuiElement::generateDraw();

	if(showName){
		textMesh = getTextMesh(getName(), textPadding, getHeight() - textPadding);
	}
}

void ofxGuiTextureInput::render(){

	ofxGuiElement::render();

	ofColor c = ofGetStyle().color;

	if(_bLoaded && graphics){
		graphics->draw(0, 0, getWidth(), getHeight());
	}

	if(showName){
		ofBlendMode blendMode = ofGetStyle().blendingMode;
		if(blendMode != OF_BLENDMODE_ALPHA){
			ofEnableAlphaBlending();
		}
		ofSetColor(textColor);

		bindFontTexture();
		textMesh.draw();
		unbindFontTexture();

		ofSetColor(c);
		if(blendMode != OF_BLENDMODE_ALPHA){
			ofEnableBlendMode(blendMode);
		}
	}
}

ofAbstractParameter & ofxGuiTextureInput::getParameter(){
	return label;
}

std::string ofxGuiTextureInput::getClassType(){
	return "graphics";
}

vector<std::string> ofxGuiTextureInput::getClassTypes(){
	vector<std::string> types = ofxGuiElement::getClassTypes();
	types.push_back(getClassType());
	return types;
}


bool ofxGuiTextureInput::mouseReleased(ofMouseEventArgs & args){
    if (isMouseOver()) {
        ofNotifyEvent(showEvent);
        selectionView->show();
    }
    return false;
}
