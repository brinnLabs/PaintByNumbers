#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {

	ofGLFWWindowSettings settings;
	settings.stencilBits = 8;
	settings.glVersionMajor = 2;
	settings.glVersionMinor = 1;
	settings.width = 1280;
	settings.height = 720;
	settings.windowMode = OF_WINDOW;

	auto window = ofCreateWindow(settings);
	auto app = make_shared<ofApp>();
	ofRunApp(window, app);

	return ofRunMainLoop();

}
