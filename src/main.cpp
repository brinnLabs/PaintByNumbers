#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {

	ofGLFWWindowSettings settings;
	settings.stencilBits = 8;
	settings.glVersionMajor = 2;
	settings.glVersionMinor = 1;
	settings.width = 1000;
	settings.height = 800;
	settings.windowMode = OF_WINDOW;

	auto window = ofCreateWindow(settings);
	auto app = make_shared<ofApp>();
	ofRunApp(window, app);

	return ofRunMainLoop();

	//ofSetupOpenGL(1024, 768, OF_WINDOW);			// <-------- setup the GL context

	//// this kicks off the running of my app
	//// can be OF_WINDOW or OF_FULLSCREEN
	//// pass in width and height too:
	//ofRunApp(new ofApp());

}
