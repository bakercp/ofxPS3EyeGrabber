//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"


void ofApp::setup()
{
    using namespace ps3eye;
    
    std::vector<PS3EYECam::PS3EYERef> devices = PS3EYECam::getDevices();
    
    for (std::size_t i = 0; i < 4/*devices.size()*/; ++i)
    {
        cameras.push_back(Camera());
        cameras.back().eye = devices.at(i);
        cameras.back().eye->init(640, 480, 60, PS3EYECam::EOutputFormat::BGR);
        cameras.back().eye->start();
        cameras.back().pix.allocate(cameras.back().eye->getWidth(),
                                    cameras.back().eye->getHeight(),
                                    OF_PIXELS_BGR);
        cameras.back().eye->setAutogain(true);
        cameras.back().eye->setAutoWhiteBalance(true);
    }
}

void ofApp::exit(){
    for (auto& eye: cameras)
    {
        eye.eye->stop();
    }
}


void ofApp::update()
{
    for (auto& eye: cameras)
    {
        eye.eye->getFrame(eye.pix.getData());
        eye.tex.loadData(eye.pix);
        eye.camFrameCount++;
        float timeNow = ofGetElapsedTimeMillis();
        if( timeNow > eye.camFpsLastSampleTime + 1000 ) {
            uint32_t framesPassed = eye.camFrameCount - eye.camFpsLastSampleFrame;
            eye.camFps = (float)(framesPassed / ((timeNow - eye.camFpsLastSampleTime)*0.001f));
            
            eye.camFpsLastSampleTime = timeNow;
            eye.camFpsLastSampleFrame = eye.camFrameCount;
        }
    }
}



void ofApp::draw()
{
    ofSetHexColor(0xffffff);

    float x = 0;
    float y = 0;
    
    for (auto& eye: cameras)
    {
        ofPushMatrix();
        ofTranslate(x, y);
        x += eye.tex.getWidth();
        if (x > ofGetWidth()) { x = 0; y += eye.tex.getHeight(); }
        
        eye.tex.draw(0, 0);

        string str = "app fps: ";
        str += ofToString(ofGetFrameRate(), 2);
        str += "\neye fps: " + ofToString(eye.camFps, 2);
        ofDrawBitmapStringHighlight(str, 10, 15);
        
        ofPopMatrix();
    }
    
    
}
