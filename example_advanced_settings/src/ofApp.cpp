//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"


void ofApp::setup()
{
    ofSetVerticalSync(true);

    grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());

    // These are all settings that can be set for any ofVideoGrabber.
    // grabber.setDeviceID(0x00000000);

    // The native pixel format for the ofxPS3EyeGrabber is OF_PIXELS_YUY2
    // (aka YUV422).  When used this way, no additional pixel copies are made
    // or colorspace conversions are performed.
    //
    // The programmable renderer is able to directly render YUV422 pixels.
    // so be sure to that the OpenGL version is > 3.2, otherwise you'll
    // get a blank screen.
    grabber.setPixelFormat(OF_PIXELS_NATIVE);
    grabber.setDesiredFrameRate(60);
    grabber.setup(640, 480);

    // These are examples of ofxPS3EyeGrabber-specific paramaters.
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
