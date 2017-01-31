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

    // Load the JSON from a configuration file.
    ofJson config;
    ofFile file("settings.json");
    file >> config;

    // Create a grabber from the JSON.
    grabber = ofxPS3EyeGrabber::fromJSON(config);
}


void ofApp::update()
{
    // Update the grabber.
    grabber->update();
}


void ofApp::draw()
{
    ofBackground(0);
    ofSetColor(255);

    grabber->draw(0, 0);

    std::stringstream ss;

    ss << " App FPS: " << ofGetFrameRate() << std::endl;
    ss << " Cam FPS: " << grabber->getGrabber<ofxPS3EyeGrabber>()->getFPS() << std::endl;
    ss << "Real FPS: " << grabber->getGrabber<ofxPS3EyeGrabber>()->getActualFPS() << std::endl;
    ss << "      id: 0x" << ofToHex(grabber->getGrabber<ofxPS3EyeGrabber>()->getDeviceId());

    ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));

}
