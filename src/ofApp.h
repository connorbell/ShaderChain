#pragma once

#include "ofMain.h"
#include "ShaderChain.h"
#include "ShaderPass.h"

class ofApp : public ofBaseApp{
	public:
		ofApp();
		~ofApp();
		float resolutionX = 500;
		float resolutionY = 500;
		ShaderChain shaderChain;

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void setup();
		void update();
		void draw();
};
