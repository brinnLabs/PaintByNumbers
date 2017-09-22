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

struct TrackedHand {
private:
	ofPoint coordinates;
	HandState state;
	int hoverCounter;

	ofImage cursorIcon;

	CursorType cursorType;
	ofColor currentColor;

	ofTexture tex;

public:
	TrackedHand(ofPoint coordinates, HandState state) {
		cursorType = HAND;
		currentColor = ofColor(255);

		ofImage paintCan;
		paintCan.load("bucket3.png");

		ofFbo frameBuf = ofFbo();
		frameBuf.allocate(35, 35);
		frameBuf.begin(); {
			paintCan.draw(0, 0, 35, 35);
		} frameBuf.end();
		tex = frameBuf.getTexture();
	}

	void TrackedHand::setHandState(HandState new_state) {
		state = new_state;
		if (state == HandState_Closed) {
			hoverCounter = ofGetElapsedTimeMillis();
		}
	}

	HandState TrackedHand::getHandState() {
		return state;
	}

	void TrackedHand::setHandCoordinate(ofPoint new_coords) {
		if (new_coords.distance(coordinates) > 5) {
			hoverCounter = ofGetElapsedTimeMillis();
		}
		coordinates = new_coords;
	}

	ofPoint TrackedHand::getHandCoordinate() {
		return coordinates;
	}

	void TrackedHand::setCurrentColor(ofColor color) {
		currentColor = color;
	}

	ofColor TrackedHand::getCurrentColor() {
		return currentColor;
	}

	void TrackedHand::setCursor(CursorType cursor) {
		cursorType = cursor;
		switch (cursorType) {
		case HAND:
			cursorIcon.load("hand.png");
			break;
		case BUCKET:
			cursorIcon.load("bucket2.png");
			break;
		case ERASER:
			cursorIcon.load("eraser.png");
			break;
		}
	}

	CursorType TrackedHand::getCursor() {
		return cursorType;
	}

	void TrackedHand::draw() {
		switch (cursorType) {
		case HAND:
		case ERASER:
			ofSetColor(255);
			break;
		case BUCKET:
			ofSetColor(currentColor);
			break;
		}
		cursorIcon.draw(coordinates, 35, 35);
	}

	void TrackedHand::drawHover() {
		ofPushStyle();
		ofPushMatrix();
		ofTranslate(coordinates.x + 17.5, coordinates.y + 17.5, 0);
		ofSetColor(ofColor::slateGray);
		ofCircle(0, 0, 30);
		ofFill();
		ofSetColor(ofColor::greenYellow);
		ofBeginShape();


		float angleStep = TWO_PI / 60;
		float radius = 28;

		ofVertex(0, 0);
		for (int i = 0; i < fmod(((ofGetElapsedTimeMillis() - hoverCounter) / 16.3), 61); i++) {
			float anglef = (i)* angleStep;
			float x = radius * sin(anglef);
			float y = radius * -cos(anglef);
			ofVertex(x, y);
		}
		ofVertex(0, 0);
		ofEndShape(OF_CLOSE);
		ofPopMatrix();
		ofPopStyle();
	}

	void TrackedHand::drawHoverColor(ofColor color) {
		ofImage img = ofImage("bucket3.png");
		ofSetColor(255);
		img.draw(coordinates.x + 7.5, coordinates.y - 42.5, 35, 35);
		ofSetColor(color);
		float yprct = 35 * ((ofGetElapsedTimeMillis() - hoverCounter) / 1000.0);
		tex.drawSubsection(coordinates.x + 7.5, coordinates.y - 42.5 + (35 - yprct), 35, yprct, 0, 35 - yprct);
	}

	int TrackedHand::getHoverCounter() {
		return hoverCounter;
	}

	int TrackedHand::getHoverPeriod() {
		return ofGetElapsedTimeMillis() - hoverCounter;
	}

	void TrackedHand::resetHoverCounter() {
		hoverCounter = ofGetElapsedTimeMillis() + 666;
	}
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

class ofApp : public ofBaseApp {

public:
	void setup2();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
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

	int currentImage;

	vector<ofColor> layerColors;

	ofxSVG drawnSvg;

	bool hasSetup;

	ofxImgButton hand, bucket, eraser, forward, back, reset;
	void buttonPressed(const pair<bool, int> & button);

	ofPoint BodyToScreen(const CameraSpacePoint & bodyPoint, int width, int height);

	HRESULT InitializeDefaultSensor();

	void ProcessBody(INT64 nTime, int nBodyCount, IBody ** ppBodies);

	ofRectangle canvas;

	ofFbo svgFrameTop, svgFrame;

	ofDirectory dir;

	// Current Kinect
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;

	// Body reader
	IBodyFrameReader*       m_pBodyFrameReader;

	vector<int> trackedHandIds;
	map<int, pair<TrackedHand, TrackedHand>> hands;
};
