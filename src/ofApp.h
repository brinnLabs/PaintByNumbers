#pragma once

#include "ofMain.h"
#include "ofxSplashScreen.h"
#include "ofxUIUtils.h"
#include "ofxSvg.h"

// Kinect Header files
#include <Kinect.h>

enum CursorType {
	HAND,
	BUCKET,
	ERASER
};

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

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
		vector<ofColor> layerColors;

		ofxSVG drawnSvg;

		bool hasSetup, showHover, toolStatus;
		ofImage cursorIcon;

		CursorType cursorType;

		ofxImgButton hand, bucket, eraser, forward, back, reset;
		void buttonPressed(const pair<bool, int> & button);

		ofPoint BodyToScreen(const CameraSpacePoint & bodyPoint, int width, int height);

		HRESULT InitializeDefaultSensor();

		void ProcessBody(INT64 nTime, int nBodyCount, IBody ** ppBodies);

		ofRectangle canvas;

		ofFbo svgFrameTop, svgFrame;

		ofDirectory dir;

		ofTexture tex;

		// Current Kinect
		IKinectSensor*          m_pKinectSensor;
		ICoordinateMapper*      m_pCoordinateMapper;

		// Body reader
		IBodyFrameReader*       m_pBodyFrameReader;

		map<int, ofPoint> handCoordinates;
};
