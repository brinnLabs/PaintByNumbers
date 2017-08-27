#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {

	/*ofGLFWWindowSettings settings;
	settings.stencilBits = 8;

	auto window = ofCreateWindow(settings);
	auto app = make_shared<ofApp>();

	ofRunApp(window, app);

	return ofRunMainLoop();*/

	ofSetupOpenGL(1024, 768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
