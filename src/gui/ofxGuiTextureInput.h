#pragma once

#include "ofxGuiElement.h"
#include "TextureInputSelectionView.h"

class ofxGuiTextureInput : public ofxGuiElement {
	public:

		ofxGuiTextureInput(std::string canvasName="", TextureInputSelectionView *selectionView = nullptr, const ofJson& config = ofJson());
		ofxGuiTextureInput(std::string canvasName, ofBaseDraws * graphics, const ofJson& config = ofJson());
		ofxGuiTextureInput(std::string canvasName, ofBaseDraws * graphics, float w, float h = 0);
		virtual ~ofxGuiTextureInput();
        TextureInputSelectionView *selectionView;

		virtual float getMinWidth() override;
		virtual float getMinHeight() override;

		void setAutoHeight();
		void setAutoWidth();

        ofEvent<void> showEvent;

		virtual void setGraphics(ofBaseDraws* graphics);

		virtual ofAbstractParameter & getParameter() override;
		virtual bool mouseReleased(ofMouseEventArgs & args) override;

		static std::string getClassType();

	protected:

		void setup(std::string canvasName = "", float w = 0, float h = 0);

		virtual std::vector<std::string> getClassTypes() override;

		bool setValue(float mx, float my, bool bCheckBounds) override {
			return false;
		}
		virtual void render() override;
		virtual void generateDraw() override;

		virtual void onResize(DOM::ResizeEventArgs& args);

		ofPath bg;
		ofVboMesh textMesh;
		ofBaseDraws * graphics;
		ofParameter <std::string> label;
		bool _bLoaded;
		bool resizing = false;
				bool autoWidth, autoHeight;

};
