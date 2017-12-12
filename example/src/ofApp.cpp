//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"


void ofApp::setup()
{
    // Set the video grabber to the ofxPS3EyeGrabber.
    grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    grabber.setup(640, 480);
}


void ofApp::update()
{
    grabber.update();
}



void ofApp::draw()
{
    ofSetColor(255);
    grabber.draw(0, 0);
}
