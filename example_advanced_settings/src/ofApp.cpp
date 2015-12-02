// =============================================================================
//
// Copyright (c) 2014-2015 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofApp.h"


void ofApp::setup()
{
    ofSetVerticalSync(true);

    std::vector<ofVideoDevice> devices = grabber.listDevices();

	if (!devices.empty())
	{
		grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
		grabber.setDeviceID(0);
		grabber.setPixelFormat(OF_PIXELS_NATIVE);
		grabber.setDesiredFrameRate(60);
		grabber.setup(640, 480);

        // Set PS3EyeGrabber specific paramaters.
		grabber.getGrabber<ofxPS3EyeGrabber>()->setAutogain(true);
		grabber.getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(true);
	}
}


void ofApp::update()
{
	grabber.update();
}


void ofApp::draw()
{
    ofBackground(0);
    ofSetColor(255);

    grabber.draw(0, 0);

    std::stringstream ss;

    ss << " App FPS: " << ofGetFrameRate() << std::endl;
	ss << " Cam FPS: " << grabber.getGrabber<ofxPS3EyeGrabber>()->getFPS()  << std::endl;
	ss << "Real FPS: " << grabber.getGrabber<ofxPS3EyeGrabber>()->getActualFPS();

    ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));

}
