//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"


void ofApp::setup()
{
    // Set the custom ofxPS3EyeGrabber.
    grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());

    // These are all settings that can be set for any ofVideoGrabber.
    // grabber.setDeviceID(0x00000000);

    // The native pixel format for the ofxPS3EyeGrabber is a Bayer pattern
    // (it will look black and white).
    grabber.setPixelFormat(OF_PIXELS_NATIVE);
    grabber.setDesiredFrameRate(75);
    grabber.setup(640, 480);

    // These are examples of ofxPS3EyeGrabber-specific paramaters.
    // These must be set after the grabber is set up.
    grabber.getGrabber<ofxPS3EyeGrabber>()->setAutogain(true);
    grabber.getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(true);
 
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
    ss << " Cam FPS: " << grabber.getGrabber<ofxPS3EyeGrabber>()->getFPS() << std::endl;
    ss << "Real FPS: " << grabber.getGrabber<ofxPS3EyeGrabber>()->getActualFPS() << std::endl;
    ss << "      id: 0x" << ofToHex(grabber.getGrabber<ofxPS3EyeGrabber>()->getDeviceId());

    ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));

}
