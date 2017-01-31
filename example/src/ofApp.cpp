//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"


void ofApp::setup()
{
    // Set the video grabber to the ofxPS3EyeGrabber.
    vidGrabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    vidGrabber.setup(640, 480);
}


void ofApp::update()
{
    vidGrabber.update();
}


void ofApp::draw()
{
    ofBackground(0);
    ofSetColor(255);
    vidGrabber.draw(0, 0);
}
