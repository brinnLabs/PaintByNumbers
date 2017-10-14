#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup2() {
	ofSetLogLevel(ofLogLevel::OF_LOG_VERBOSE);
	this->splashScreen.init("paintbynumbers.jpg");
	this->splashScreen.begin();

	font.loadFont(OF_TTF_SANS, 14);

	ofHideCursor();
	InitializeDefaultSensor();


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

	colorPick.setup(VERTICAL_HALF_BRIGHTNESS, ofGetWidth() / 5, 3 * (ofGetHeight() / 4));

	canvas = ofRectangle(ofGetWidth() / 5 + ofGetWidth() / 25, ofGetHeight() / 25, ofGetWidth() * (4.0 / 5.0) - 2 * (ofGetWidth() / 25), ofGetHeight() * (23.0 / 25.0));

	shivaRenderer = ofPtr<ofxShivaVGRenderer>(new ofxShivaVGRenderer);
	ofSetCurrentRenderer(shivaRenderer);

	shivaRenderer->setLineCapStyle(VG_CAP_SQUARE);
	shivaRenderer->setLineJoinStyle(VG_JOIN_MITER);

	ofEnableSmoothing();
	ofSetVerticalSync(false);
	ofSetFullscreen(false);
	ofSetCircleResolution(50);
	ofSetCurveResolution(50);

	hand.setup("hand.png");
	reset.setup("Reset.png");
	bucket.setup("bucket3.png");
	eraser.setup("eraser.png");
	forward.setup("forward.png");
	back.setup("back.png");

	svgFrameTop.allocate(canvas.width, canvas.height);
	svgFrameTop.begin(); {
		ofClear(255);
	} svgFrameTop.end();

	svgFrame.allocate(canvas.width, canvas.height);
	svgFrame.begin(); {
		ofPushMatrix(); {
			ofTranslate(-canvas.x, -canvas.y);
			float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
			ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
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
	else if (hasSetup) {
		if (m_pBodyFrameReader)
		{
			IBodyFrame* pBodyFrame = NULL;

			HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

			if (SUCCEEDED(hr))
			{
				IBody* ppBodies[BODY_COUNT] = { 0 };

				if (SUCCEEDED(hr))
				{
					hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
				}

				if (SUCCEEDED(hr))
				{
					ProcessBody(BODY_COUNT, ppBodies);
				}

				for (int i = 0; i < _countof(ppBodies); ++i)
				{
					SafeRelease(ppBodies[i]);
				}
			}


			SafeRelease(pBodyFrame);
		}
		for (auto id : trackedHandIds) {
			TrackedHand & lhand = hands.at(id).first; {
				if (lhand.getHandState() == HandState_Closed && lhand.getHoverPeriod() > 1000) {
					lhand.resetHoverCounter();
					if (hand.getBoundingRect().inside(lhand.getHandCoordinate())) {
						lhand.setCursor(HAND);
					}
					else if (eraser.getBoundingRect().inside(lhand.getHandCoordinate())) {
						lhand.setCursor(ERASER);
					}
					else if (bucket.getBoundingRect().inside(lhand.getHandCoordinate())) {
						lhand.setCursor(BUCKET);
					}
					else if (reset.getBoundingRect().inside(lhand.getHandCoordinate())) {
						svgFrameTop.begin(); {
							ofClear(255);
						} svgFrameTop.end();
						for (auto & layer : layerColors) {
							layer = ofColor::white;
						}
					}
					else if (forward.getBoundingRect().inside(lhand.getHandCoordinate())) {
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
								ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
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
					else if (back.getBoundingRect().inside(lhand.getHandCoordinate())) {
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
								ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
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
					else if (ofRectangle(0, 0, ofGetWidth() / 5, 3 * (ofGetHeight() / 4)).inside(lhand.getHandCoordinate())) {
						lhand.setCurrentColor(colorPick.getColorAt(lhand.getHandCoordinate()));
					}
					else if (canvas.inside(lhand.getHandCoordinate())) {
						if (lhand.getCursor() != BUCKET && lhand.getCursor() != ERASER) {
							//do nothing currently
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
								if (fullPoly.size() > 0 && fullPoly.inside(lhand.getHandCoordinate().x - ((ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2)), lhand.getHandCoordinate().y - ((ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2)))) {
									if (lhand.getCursor() == BUCKET) {
										layerColors[counter] = lhand.getCurrentColor();
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
										ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
										path.draw();
									} ofPopMatrix();
								} svgFrameTop.end();
								counter++;
							}
						}
					}
				}
			}
			TrackedHand  & rhand = hands.at(id).second; {
				if (rhand.getHandState() == HandState_Closed && rhand.getHoverPeriod() > 1000) {
					rhand.resetHoverCounter();
					if (hand.getBoundingRect().inside(rhand.getHandCoordinate())) {
						rhand.setCursor(HAND);
					}
					else if (eraser.getBoundingRect().inside(rhand.getHandCoordinate())) {
						rhand.setCursor(ERASER);
					}
					else if (bucket.getBoundingRect().inside(rhand.getHandCoordinate())) {
						rhand.setCursor(BUCKET);
					}
					else if (reset.getBoundingRect().inside(rhand.getHandCoordinate())) {
						svgFrameTop.begin(); {
							ofClear(255);
						} svgFrameTop.end();
						for (auto & layer : layerColors) {
							layer = ofColor::white;
						}
					}
					else if (forward.getBoundingRect().inside(rhand.getHandCoordinate())) {
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
								ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
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
					else if (back.getBoundingRect().inside(rhand.getHandCoordinate())) {
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
								ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
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
					else if (ofRectangle(0, 0, ofGetWidth() / 5, 3 * (ofGetHeight() / 4)).inside(rhand.getHandCoordinate())) {
						rhand.setCurrentColor(colorPick.getColorAt(rhand.getHandCoordinate()));
					}
					else if (canvas.inside(rhand.getHandCoordinate())) {
						if (rhand.getCursor() != BUCKET && rhand.getCursor() != ERASER) {
							//do nothing currently
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
								if (fullPoly.size() > 0 && fullPoly.inside(rhand.getHandCoordinate().x - ((ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2)), rhand.getHandCoordinate().y - ((ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2)))) {
									if (rhand.getCursor() == BUCKET) {
										layerColors[counter] = rhand.getCurrentColor();
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
										ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
										path.draw();
									} ofPopMatrix();
								} svgFrameTop.end();
								counter++;
							}
						}
					}
				}
			}

		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (hasSetup) {
		ofSetColor(255);
		ofRect(canvas);

		svgFrame.draw(canvas.x, canvas.y);
		svgFrameTop.draw(canvas.x, canvas.y);

		hand.draw(0, ofGetHeight() * (3.0 / 4.0), ofGetWidth() / 10, ofGetHeight() / 8);
		bucket.draw(ofGetWidth() / 10, ofGetHeight() * (3.0 / 4.0), ofGetWidth() / 10, ofGetHeight() / 8);
		eraser.draw(0, ofGetHeight() * (7.0 / 8.0), ofGetWidth() / 10, ofGetHeight() / 8);
		reset.draw(ofGetWidth() / 10, ofGetHeight() * (7.0 / 8.0), ofGetWidth() / 10, ofGetHeight() / 8);
		forward.draw(ofGetWidth() * (24 / 25.0), canvas.getCenter().y - 50, ofGetWidth() / 25, 100);
		back.draw(ofGetWidth() / 5, canvas.getCenter().y - 50, ofGetWidth() / 25, 100);

		colorPick.draw(0, 0, ofGetWidth() / 5, 3 * (ofGetHeight() / 4));
		ofRectangle colorRect = ofRectangle(0, 0, ofGetWidth() / 5, 3 * (ofGetHeight() / 4));

		for (auto id : trackedHandIds) {
			TrackedHand lhand = hands.at(id).first; {
				if (colorRect.inside(lhand.getHandCoordinate())) {
					lhand.drawHoverColor(colorPick.getColorAt(lhand.getHandCoordinate()));
				}

				else {
					if (lhand.getHandState() == HandState_Closed && lhand.getHoverPeriod() < 1000 && lhand.getHoverPeriod() > 0) {
						lhand.drawHover();
					}
					lhand.draw();
				}
			}
			TrackedHand rhand = hands.at(id).second; {
				if (colorRect.inside(rhand.getHandCoordinate())) {
						rhand.drawHoverColor(colorPick.getColorAt(rhand.getHandCoordinate()));
				}

				else {
					if (rhand.getHandState() == HandState_Closed && rhand.getHoverPeriod() < 1000 && rhand.getHoverPeriod() > 0) {
						rhand.drawHover();
					}
					rhand.draw();
				}
			}
		}

	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'f') {
		ofToggleFullscreen();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
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
	colorPick.setup(VERTICAL_HALF_BRIGHTNESS, ofGetWidth() / 5, 3 * (ofGetHeight() / 4));

	canvas = ofRectangle(ofGetWidth() / 5 + ofGetWidth() / 25, ofGetHeight() / 25, ofGetWidth() * (4.0 / 5.0) - 2 * (ofGetWidth() / 25), ofGetHeight() * (23.0 / 25.0));

	svgFrameTop.allocate(canvas.width, canvas.height);
	svgFrame.allocate(canvas.width, canvas.height);
	svgFrameTop.begin(); {
		ofClear(255);
	} svgFrameTop.end();

	svgFrame.allocate(canvas.width, canvas.height);
	svgFrame.begin(); {
		ofPushMatrix(); {
			ofTranslate(-canvas.x, -canvas.y);
			float scale = MIN(canvas.width / drawnSvg.getWidth(), canvas.height / drawnSvg.getHeight());
			ofTranslate(ofGetWidth() / 5 + ofGetWidth() / 25 + canvas.width / 2 - (drawnSvg.getWidth()*scale) / 2, ofGetHeight() / 25 + canvas.height / 2 - (drawnSvg.getHeight()*scale) / 2);
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
	ColorSpacePoint colorPoint = { 0 };
	m_pCoordinateMapper->MapCameraPointToColorSpace(bodyPoint, &colorPoint);

	/*An RGB color camera – 640×480 in version 1, 1920×1080 in version 2
	A depth sensor – 320×240 in v1, 512×424 in v2
	An infrared sensor – 512×424 in v2*/

	float screenPointX = ofMap(colorPoint.X, 400, 1520, 0, width, true);
	float screenPointY = ofMap(colorPoint.Y, 400, 900, 0, height, true);

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
		ofLogError("No Kinect found!");
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
/// <param name="nBodyCount">body data count</param>
/// <param name="ppBodies">body data in frame</param>
/// </summary>
void ofApp::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	trackedHandIds.clear();
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			BOOLEAN bTracked = false;
			HRESULT hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				UINT64 id_num = 0;
				pBody->get_TrackingId(&id_num);
				trackedHandIds.push_back(id_num);
				Joint joints[JointType_Count];
				ofPoint jointPoints[JointType_Count];
				HandState leftHandState = HandState_Unknown;
				HandState rightHandState = HandState_Unknown;

				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					jointPoints[JointType_HandLeft] = BodyToScreen(joints[JointType_HandLeft].Position, ofGetWidth(), ofGetHeight());
					jointPoints[JointType_HandRight] = BodyToScreen(joints[JointType_HandRight].Position, ofGetWidth(), ofGetHeight());

					if (hands.count(id_num) > 0) {
						hands.at(id_num).first.setHandCoordinate(jointPoints[JointType_HandLeft]);
						hands.at(id_num).first.setHandState(leftHandState);
						hands.at(id_num).second.setHandCoordinate(jointPoints[JointType_HandRight]);
						hands.at(id_num).second.setHandState(rightHandState);
					}
					else {
						ofLogNotice("Creating new hand with id: " + ofToString(id_num));
						TrackedHand leftHand = TrackedHand(jointPoints[JointType_HandLeft], leftHandState);
						TrackedHand rightHand = TrackedHand(jointPoints[JointType_HandRight], rightHandState);
						hands.emplace(id_num, make_pair(leftHand, rightHand));
					}
				}
			}
		}
	}
}