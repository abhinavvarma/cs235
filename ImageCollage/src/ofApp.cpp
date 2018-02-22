
//--------------------------------------------------------------
//
//  CS235 - User Interface Design - Kevin M. Smith
//
//  Example of how to implement single selection of images which
//  are drag and dropped into viewport.
// 
//  


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
//	ofEnableDepthTest();
	ofEnableAlphaBlending();

	// this sets the camera's distance from the object
	cam.setDistance(1000);
	cam.disableMouseInput(); 
	//cam.disableRotation();
	cam.setFarClip(-100);

	//
	// set when <control> key is held down
	//
	selectedImage = NULL;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	cam.begin();
	ofBackground(0);
	ofFill();

	for (int i = 0; i < images.size(); i++ ) {
		images[i]->draw();
	}
	cam.end();
}

//
// delete all images in list on exit
//
void ofApp::exit() {
	for (int i = 0; i < images.size(); i++) {
		delete images[i];
	}
}

void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;

	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'r':
		cam.reset();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		break;
	}
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {

	case OF_KEY_ALT:
		cam.disableMouseInput();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	ofPoint mouse_cur = ofPoint(x, y);
	ofVec3f delta = mouse_cur - mouse_last;
	selectedImage->trans.x += delta.x;
	selectedImage->trans.y -= delta.y;
	mouse_last = mouse_cur;
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	mouse_last = ofPoint(x, y);
	renderSelection();
	processSelection(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
//
// Simple file drag and drop implementation.
//
void ofApp::dragEvent(ofDragInfo dragInfo){ 

	Image *imageObj = new Image();
	imageObj->trans = cam.screenToWorld(dragInfo.position);
	if (imageObj->image.load(dragInfo.files[0]) == true)
		images.push_back(imageObj);
	else {
		cout << "Can't load image: " << dragInfo.files[0] << endl;
		delete imageObj;
	}
}

//
// Render for the purposes of selection hit testing.  In this case
// we use the color method. We render echo object as a different
// value of r,g,b.  We then compare the pixel under the mouse. The
// value is the index into the image list;
//
void ofApp::renderSelection() {
	cam.begin();
	ofBackground(0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for (int i = 0; i < images.size(); i++) {
		images[i]->draw(true, i);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	cam.end();
}

void ofApp::processSelection(int x, int y) {
	unsigned char res[4];
	GLint viewport[4];

	// read pixel under mouse x y
	//
	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

	if (selectedImage) {
		selectedImage->bSelected = false;
		selectedImage = NULL;
	}

	//
	//  lookup image by color of pixel
	//
	if (res[0] > 0 && res[0] <= images.size()) {
		Image *image = images[res[0] - 1];
		image->bSelected = true;
		selectedImage = image;

		// move selected image to beginning of list
		//
		images.erase(images.begin() + (res[0] - 1) );
		images.push_back(image);
	}

}

Image::Image()
{
	trans.x = 0;
	trans.y = 0;
	scale.x = 1.0;
	scale.y = 1.0;
	rot = 0;
	bSelected = false;
}

void Image::draw(bool bSelectMode, int index) {
	ofPushMatrix();
	ofTranslate(trans);
	ofScale(scale);
	ofRotate(rot);

	//
	// if drawing image normally
	//
	if (!bSelectMode) {

		//
		// if selected, draw a light yellowo box around image
		//
		if (bSelected) {
			ofNoFill();
			ofSetColor(255, 255, 0);
			ofSetLineWidth(5);
			ofDrawRectangle(-image.getWidth() / 2.0, -image.getHeight() / 2.0,
				image.getWidth(), image.getHeight());
		}
		ofSetColor(255, 255, 255, 255);
		image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
	
	}
	//
	// else if drawing to test selection
	//
	else {
		ofFill();
		ofSetColor(index + 1, 0, 0);
		ofDrawRectangle(-image.getWidth() / 2.0, -image.getHeight() / 2.0,
			image.getWidth(), image.getHeight());
	}
	ofPopMatrix();
}

