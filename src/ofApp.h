#pragma once

#include "ofMain.h"
#include "ofxSplashScreen.h"
#include "ofxShivaVGRenderer.h" 
#include "ofxUIUtils.h"
#include "ofxSvg.h"

enum CursorType {
	HAND,
	BRUSH,
	BUCKET,
	ERASER
};

class ofApp : public ofBaseApp{

	public:
		void setup2();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofxSplashScreen splashScreen;

		ofTrueTypeFont font;

		ofxColorPicker colorPick;
		void pickerSelected(const ofColor & color);
		void pickerMoved(const ofColor & color);

		int hoverCounter, moveCooldown, currentImage;
		ofColor movedColor, currentColor;

		ofxSVG drawnSvg;

		bool hasSetup, showHover, toolStatus;
		ofImage cursorIcon;

		CursorType cursorType;

		ofxImgButton hand, brush, bucket, eraser, forward, back;
		void buttonPressed(const pair<bool, int> & button);

		ofRectangle canvas;

		ofFbo canvasFrame, svgFrame;

		ofDirectory dir;

		ofTexture tex;

};
