#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup2() {
	ofSetLogLevel(ofLogLevel::OF_LOG_VERBOSE);
	this->splashScreen.init("paintbynumbers.jpg");
	this->splashScreen.begin();

	font.loadFont(OF_TTF_SANS, 14);

	ofHideCursor();
	InitializeDefaultSensor();

	cursorIcon.load("bucket3.png");
	cursorType = HAND;

	ofFbo frameBuf = ofFbo();
	frameBuf.allocate(35, 35);
	frameBuf.begin(); {
		cursorIcon.draw(0, 0, 35, 35);
	} frameBuf.end();
	tex = frameBuf.getTexture();

	cursorIcon.load("hand.png");

	string path = "svgs";
	dir.setShowHidden(false);
	dir.open(path);
	dir.allowExt("svg");
	//populate the directory object
	dir.listDir();

	if (dir.numFiles() > 0) {
		drawnSvg.load("svgs\\" + dir.getName(0));
	}

	for (auto path : drawnSvg.getPaths()) {
		layerColors.push_back(ofColor::white);
	}

	currentImage = 0;
	toolStatus = true;

	colorPick.setup(HORIZONTAL_HALF_BRIGHTNESS, ofGetWidth() / 2, ofGetHeight() / 5);

	canvas = ofRectangle(ofGetWidth() / 25, ofGetHeight() / 25, ofGetWidth() * (23.0 / 25.0), ofGetHeight() * .8 - 2 * (ofGetHeight() / 25));

	ofEnableSmoothing();
	ofSetVerticalSync(false);
	ofSetFullscreen(false);
	ofSetCircleResolution(50);
	ofSetCurveResolution(50);

	hand.setup("hand.png");
	hand.setColor(ofColor::mediumSeaGreen);
	hand.setID(1);
	reset.setup("Reset.png");
	reset.setID(2);
	bucket.setup("bucket3.png");
	bucket.setID(3);
	eraser.setup("eraser.png");
	eraser.setID(4);
	forward.setup("forward.png");
	forward.setID(5);
	back.setup("back.png");
	back.setID(6);

	svgFrameTop.allocate(canvas.width, canvas.height);
	svgFrameTop.begin(); {
		ofClear(255);
	} svgFrameTop.end();

	svgFrame.allocate(canvas.width, canvas.height);
	svgFrame.begin(); {
		ofPushMatrix(); {
			ofTranslate(-canvas.x, -canvas.y);
			float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
			ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
			for (auto path : drawnSvg.getPaths()) {
				path.setFilled(true);
				path.setFillColor(ofColor::white);
				path.setStrokeWidth(1);
				path.setStrokeColor(ofColor::black);
				path.scale(scale, scale);
				path.draw();
			}
		} ofPopMatrix();
	} svgFrame.end();

	ofBackground(ofColor::whiteSmoke);

	ofAddListener(colorPick.pickerPickEvent, this, &ofApp::pickerSelected);
	ofAddListener(colorPick.pickerMoveEvent, this, &ofApp::pickerMoved);
	ofAddListener(hand.buttonEvent, this, &ofApp::buttonPressed);
	ofAddListener(bucket.buttonEvent, this, &ofApp::buttonPressed);
	ofAddListener(eraser.buttonEvent, this, &ofApp::buttonPressed);
	ofAddListener(forward.buttonEvent, this, &ofApp::buttonPressed);
	ofAddListener(back.buttonEvent, this, &ofApp::buttonPressed);

	ofLogNotice("Finished Setup Process");
	hasSetup = true;
	this->splashScreen.end();
}

//--------------------------------------------------------------
void ofApp::update() {
	if (ofGetFrameNum() == 0) {
		hasSetup = false;
	} if (ofGetFrameNum() == 1) {
		this->setup2();
	}
	else {
		if (m_pBodyFrameReader)
		{


			IBodyFrame* pBodyFrame = NULL;

			HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

			if (SUCCEEDED(hr))
			{
				INT64 nTime = 0;

				hr = pBodyFrame->get_RelativeTime(&nTime);

				IBody* ppBodies[BODY_COUNT] = { 0 };

				if (SUCCEEDED(hr))
				{
					hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
				}

				if (SUCCEEDED(hr))
				{
					ProcessBody(nTime, BODY_COUNT, ppBodies);
				}

				for (int i = 0; i < _countof(ppBodies); ++i)
				{
					SafeRelease(ppBodies[i]);
				}
			}
		

		SafeRelease(pBodyFrame);
		}
		if (ofGetElapsedTimeMillis() - moveCooldown > 666) {
			if (hand.getIsHovering() || bucket.getIsHovering() || eraser.getIsHovering()) {
				switch (cursorType) {
				case HAND:
					if (!hand.getIsHovering()) {
						if (!showHover) {
							hoverCounter = ofGetElapsedTimeMillis();
							showHover = true;
						}
					}
					break;
				case ERASER:
					if (!eraser.getIsHovering()) {
						if (!showHover) {
							hoverCounter = ofGetElapsedTimeMillis();
							showHover = true;
						}
					}
					break;
				case BUCKET:
					if (!bucket.getIsHovering()) {
						if (!showHover) {
							hoverCounter = ofGetElapsedTimeMillis();
							showHover = true;
						}
					}
					break;
				}
			}
			else if (forward.getIsHovering() || back.getIsHovering() || reset.getIsHovering()) {
				if (!showHover) {
					hoverCounter = ofGetElapsedTimeMillis();
					showHover = true;
				}
			}
			else if (ofRectangle(0, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 2, ofGetHeight() / 5).inside(mouseX, mouseY)) {
				if (!showHover && movedColor != currentColor) {
					hoverCounter = ofGetElapsedTimeMillis();
					showHover = true;
				}
			}
			else if (canvas.inside(mouseX, mouseY)) {
				if (!showHover && cursorType != HAND) {
					hoverCounter = ofGetElapsedTimeMillis();
					showHover = true;
				}
			}
			else {
				showHover = false;
			}

			if (showHover && ofGetElapsedTimeMillis() - hoverCounter > 1000) {
				showHover = false;
				if (hand.getIsHovering()) {
					cursorIcon.load("hand.png");
					cursorType = HAND;
					hand.setColor(ofColor::mediumSeaGreen);
					eraser.setColor(255);
					bucket.setColor(255);
				}
				else if (eraser.getIsHovering()) {
					cursorIcon.load("eraser.png");
					cursorType = ERASER;
					hand.setColor(255);
					eraser.setColor(ofColor::mediumSeaGreen);
					bucket.setColor(255);
				}
				else if (bucket.getIsHovering()) {
					cursorIcon.load("bucket2.png");
					cursorType = BUCKET;
					hand.setColor(255);
					eraser.setColor(255);
					bucket.setColor(ofColor::mediumSeaGreen);
				}
				else if (reset.getIsHovering()) {
					moveCooldown = ofGetElapsedTimeMillis();
					svgFrameTop.begin(); {
						ofClear(255);
					} svgFrameTop.end();
					for (auto & layer : layerColors) {
						layer = ofColor::white;
					}
				}
				else if (forward.getIsHovering()) {
					currentImage++;
					currentImage %= dir.numFiles();
					drawnSvg.load("svgs\\" + dir.getName(currentImage));
					svgFrameTop.begin(); {
						ofClear(255);
					} svgFrameTop.end();
					layerColors.clear();
					svgFrame.begin(); {
						ofClear(255);
						ofPushMatrix(); {
							ofTranslate(-canvas.x, -canvas.y);
							float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
							ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
							for (auto path : drawnSvg.getPaths()) {
								path.setFilled(true);
								path.setFillColor(ofColor::white);
								path.setStrokeWidth(1);
								path.setStrokeColor(ofColor::black);
								path.scale(scale, scale);
								path.draw();
								layerColors.push_back(ofColor::white);
							}
						} ofPopMatrix();
					} svgFrame.end();
				}
				else if (back.getIsHovering()) {
					currentImage--;
					if (currentImage < 0) {
						currentImage = dir.numFiles() - 1;
					}
					drawnSvg.load("svgs\\" + dir.getName(currentImage));
					svgFrameTop.begin(); {
						ofClear(255);
					} svgFrameTop.end();
					layerColors.clear();
					svgFrame.begin(); {
						ofClear(255);
						ofPushMatrix(); {
							ofTranslate(-canvas.x, -canvas.y);
							float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
							ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
							for (auto path : drawnSvg.getPaths()) {
								path.setFilled(true);
								path.setFillColor(ofColor::white);
								path.setStrokeWidth(1);
								path.setStrokeColor(ofColor::black);
								path.scale(scale, scale);
								path.draw();
								layerColors.push_back(ofColor::white);
							}
						} ofPopMatrix();
					} svgFrame.end();
				}
				else if (ofRectangle(0, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 2, ofGetHeight() / 5).inside(mouseX, mouseY)) {
					currentColor = movedColor;
					toolStatus = false;
				}
				else if (canvas.inside(mouseX, mouseY)) {
					if (cursorType != BUCKET && cursorType != ERASER) {
						toolStatus = !toolStatus;
					}
					else {
						//paint the first reverse layer
						auto svgVec = drawnSvg.getPaths();
						reverse(svgVec.begin(), svgVec.end());
						float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
						int counter = 0;
						for (auto path : svgVec) {
							path.setStrokeWidth(1);
							path.scale(scale, scale);
							ofPolyline fullPoly = ofPolyline();
							for (auto polyline : path.getOutline()) {
								fullPoly.addVertices(polyline.getVertices());
							}
							fullPoly.setClosed(true);
							if (fullPoly.size() > 0 && fullPoly.inside(mouseX - ((ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2)), mouseY - ((ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2)))) {
								if (cursorType == BUCKET) {
									layerColors[counter] = currentColor;
								}
								else {
									layerColors[counter] = ofColor::white;
								}
								break;
							}
							counter++;
						}
						counter = 1;
						for (auto path : drawnSvg.getPaths()) {
							path.scale(scale, scale);
							path.setFilled(true);
							path.setStrokeWidth(1);
							path.setStrokeColor(ofColor::black);
							path.setFillColor(layerColors[drawnSvg.getPaths().size() - counter]);
							svgFrameTop.begin(); {
								ofPushMatrix(); {
									ofTranslate(-canvas.x, -canvas.y);
									ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
									path.draw();
								} ofPopMatrix();
							} svgFrameTop.end();
							counter++;
						}
					}
					moveCooldown = ofGetElapsedTimeMillis();
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (hasSetup) {
		ofSetColor(255);

		ofSetColor(0);
		ofDrawBitmapString("FPS: " + ofToString(ofGetFrameRate()), 20, 20);

		ofSetColor(255);
		ofRect(canvas);

		svgFrame.draw(canvas.x, canvas.y);
		svgFrameTop.draw(canvas.x, canvas.y);

		hand.draw(ofGetWidth() / 2, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 8, ofGetHeight() / 5);
		bucket.draw(ofGetWidth() / 2 + ofGetWidth() / 8, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 8, ofGetHeight() / 5);
		eraser.draw(ofGetWidth() / 2 + 2 * ofGetWidth() / 8, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 8, ofGetHeight() / 5);
		reset.draw(ofGetWidth() / 2 + 3 * ofGetWidth() / 8, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 8, ofGetHeight() / 5);
		forward.draw(ofGetWidth() * (24 / 25.0), canvas.getCenter().y - 50, ofGetWidth() / 25, 100);
		back.draw(0, canvas.getCenter().y - 50, ofGetWidth() / 25, 100);

		colorPick.draw(0, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 2, ofGetHeight() / 5);
		ofRectangle colorRect = ofRectangle(0, ofGetHeight() * (4.0 / 5.0), ofGetWidth() / 2, ofGetHeight() / 5);
		if (colorRect.inside(mouseX, mouseY)) {
			ofSetColor(ofColor::darkGray);
			ofBeginShape(); {
				ofVertex(mouseX, mouseY);
				ofVertex(mouseX + 3, mouseY - 42);
				ofVertex(mouseX + 42, mouseY - 3);
			} ofEndShape(OF_CLOSE);
			ofRectRounded(mouseX + 3, mouseY - 47, 44, 44, 2);
			ofSetColor(movedColor);
			ofRectRounded(mouseX + 5, mouseY - 45, 40, 40, 2);
			if (movedColor == currentColor) {
				tex.draw(mouseX + 7.5, mouseY - 42.5, 35, 35);
			}
		}


		if (showHover) {
			if (!colorRect.inside(mouseX, mouseY)) {
				ofPushStyle();
				ofPushMatrix();
				ofTranslate(mouseX + 17.5, mouseY + 17.5, 0);
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
			else {
				ofImage img = ofImage("bucket3.png");
				ofSetColor(255);
				img.draw(mouseX + 7.5, mouseY - 42.5, 35, 35);
				ofSetColor(movedColor);
				float yprct = 35 * ((ofGetElapsedTimeMillis() - hoverCounter) / 1000.0);
				tex.drawSubsection(mouseX + 7.5, mouseY - 42.5 + (35 - yprct), 35, yprct, 0, 35 - yprct);
			}
		}

		if (!colorRect.inside(mouseX, mouseY)) {
			switch (cursorType) {
			case HAND:
			case ERASER:
				ofSetColor(255);
				break;
			case BUCKET:
				ofSetColor(currentColor);
				break;
			}
			cursorIcon.draw(mouseX, mouseY, 35, 35);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	moveCooldown = ofGetElapsedTimeMillis();
	showHover = false;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	colorPick.setup(HORIZONTAL_HALF_BRIGHTNESS, ofGetWidth() / 2, ofGetHeight() / 5);
	canvas = ofRectangle(ofGetWidth() / 25, ofGetHeight() / 25, ofGetWidth() * (23.0 / 25.0), ofGetHeight() * .8 - 2 * (ofGetHeight() / 25));
	svgFrameTop.allocate(canvas.width, canvas.height);
	svgFrame.allocate(canvas.width, canvas.height);
	svgFrameTop.begin(); {
		ofClear(255);
	} svgFrameTop.end();
	svgFrame.begin(); {
		ofClear(255);
		ofPushMatrix(); {
			ofTranslate(-canvas.x, -canvas.y);
			float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
			ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
			for (auto path : drawnSvg.getPaths()) {
				path.setFilled(true);
				path.setFillColor(ofColor::white);
				path.setStrokeWidth(1);
				path.setStrokeColor(ofColor::black);
				path.scale(scale, scale);
				path.draw();
			}
		} ofPopMatrix();
	} svgFrame.end();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::pickerSelected(const ofColor & color) {
	currentColor = color;
}

void ofApp::pickerMoved(const ofColor & color) {
	movedColor = color;
}

void ofApp::buttonPressed(const pair<bool, int>& button)
{
	if (button.first) {
		switch (button.second) {
		case 1:
			cursorIcon.load("hand.png");
			cursorType = HAND;
			showHover = false;
			hand.setColor(ofColor::mediumSeaGreen);
			eraser.setColor(255);
			bucket.setColor(255);
			break;
		case 3:
			cursorIcon.load("bucket.png");
			cursorType = BUCKET;
			showHover = false;
			hand.setColor(255);
			eraser.setColor(255);
			bucket.setColor(ofColor::mediumSeaGreen);
			break;
		case 4:
			cursorIcon.load("eraser.png");
			cursorType = ERASER;
			showHover = false;
			hand.setColor(255);
			eraser.setColor(ofColor::mediumSeaGreen);
			bucket.setColor(255);
			break;
		case 5:
			currentImage++;
			currentImage %= dir.numFiles();
			drawnSvg.load("svgs\\" + dir.getName(currentImage));
			svgFrameTop.begin(); {
				ofClear(255);
			} svgFrameTop.end();
			layerColors.clear();
			svgFrame.begin(); {
				ofClear(255);
				ofPushMatrix(); {
					ofTranslate(-canvas.x, -canvas.y);
					float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
					ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
					for (auto path : drawnSvg.getPaths()) {
						path.setFilled(true);
						path.setFillColor(ofColor::white);
						path.setStrokeWidth(1);
						path.setStrokeColor(ofColor::black);
						path.scale(scale, scale);
						path.draw();
						layerColors.push_back(ofColor::white);
					}
				} ofPopMatrix();
			} svgFrame.end();
			break;
		case 6:
			currentImage--;
			if (currentImage < 0) {
				currentImage = dir.numFiles() - 1;
			}
			drawnSvg.load("svgs\\" + dir.getName(currentImage));
			svgFrameTop.begin(); {
				ofClear(255);
			} svgFrameTop.end();
			layerColors.clear();
			svgFrame.begin(); {
				ofClear(255);
				ofPushMatrix(); {
					ofTranslate(-canvas.x, -canvas.y);
					float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
					ofTranslate(ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth() * scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight() * scale) / 2);
					for (auto path : drawnSvg.getPaths()) {
						path.setFilled(true);
						path.setFillColor(ofColor::white);
						path.setStrokeWidth(1);
						path.setStrokeColor(ofColor::black);
						path.scale(scale, scale);
						path.draw();
						layerColors.push_back(ofColor::white);
					}
				} ofPopMatrix();
			} svgFrame.end();
			break;
		}
	}
}

/// <summary>
/// Converts a body point to screen space
/// </summary>
/// <param name="bodyPoint">body point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
ofPoint ofApp::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
	// Calculate the body's position on the screen
	DepthSpacePoint depthPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

	float screenPointX = static_cast<float>(depthPoint.X * width) / ofGetWidth();
	float screenPointY = static_cast<float>(depthPoint.Y * height) / ofGetHeight();

	return ofPoint(screenPointX, screenPointY);
}

/// <summary>
/// Initializes the default Kinect sensor
/// </summary>
/// <returns>indicates success or failure</returns>
HRESULT ofApp::InitializeDefaultSensor()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		SafeRelease(pBodyFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		ofLogError("No ready Kinect found!");
		return E_FAIL;
	}

	return hr;
}

/// <summary>
/// Handle new body data
/// <param name="nTime">timestamp of frame</param>
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void ofApp::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			HRESULT hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];
				ofPoint jointPoints[JointType_Count];
				HandState leftHandState = HandState_Unknown;
				HandState rightHandState = HandState_Unknown;

				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				hr = pBody->GetJoints(_countof(joints), joints);
				UINT64 id_num = 0;
				pBody->get_TrackingId(&id_num);
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						jointPoints[j] = BodyToScreen(joints[j].Position, ofGetWidth(), ofGetHeight());
					}

					switch (leftHandState)
					{
					case HandState_Closed:
						if (handCoordinates.count(id_num) > 0) {
							handCoordinates.at(id_num) = make_pair(true, jointPoints[JointType_HandLeft]);
						}
						else {
							handCoordinates.emplace(id_num, make_pair(true, jointPoints[JointType_HandLeft]));
						}
						break;

					default:
						if (handCoordinates.count(id_num) > 0) {
							handCoordinates.at(id_num) = make_pair(false, jointPoints[JointType_HandLeft]);
						}
						else {
							handCoordinates.emplace(id_num, make_pair(false, jointPoints[JointType_HandLeft]));
						}
						break;
					}

					switch (rightHandState)
					{
					case HandState_Closed:
						if (handCoordinates.count(id_num) > 0) {
							handCoordinates.at(id_num) = make_pair(true, jointPoints[JointType_HandRight]);
						}
						else {
							handCoordinates.emplace(id_num, make_pair(true, jointPoints[JointType_HandRight]));
						}
						break;

					default:
						if (handCoordinates.count(id_num) > 0) {
							handCoordinates.at(id_num) = make_pair(false, jointPoints[JointType_HandRight]);
						}
						else {
							handCoordinates.emplace(id_num, make_pair(false, jointPoints[JointType_HandRight]));
						}
						break;
					}
				}
			}
		}
	}
}