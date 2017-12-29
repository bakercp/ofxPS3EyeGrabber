//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"


void ofApp::setup()
{
    grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    grabber.setDesiredFrameRate(300);
    grabber.setup(320, 240);

    // These are examples of ofxPS3EyeGrabber-specific paramaters.
    // These must be set after the grabber is set up.
    grabber.getGrabber<ofxPS3EyeGrabber>()->setAutogain(true);
    grabber.getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(true);
}


void ofApp::update()
{
    grabber.update();
    
    if (grabber.isFrameNew())
    {
        if (isRecording)
        {
            auto frames = grabber.getGrabber<ofxPS3EyeGrabber>()->getAllFrames();
        
            for (auto& frame: frames)
            {
                ofTexture tex;
                tex.loadData(frame.pixels);
                frameBuffer.push_back(tex);
            }
        }
    }
}


void ofApp::draw()
{
    ofBackground(0);
    ofSetColor(255);

    grabber.draw(0, 0);

    if (!isRecording)
    {
        if (!frameBuffer.empty())
        {
            currentFrame = (currentFrame + 1) % frameBuffer.size();
            frameBuffer[currentFrame].draw(grabber.getWidth(), 0);
            ofDrawBitmapString(ofToString(currentFrame) + "/" + ofToString(frameBuffer.size()), 14, grabber.getHeight() - 28);
        }

        ofDrawBitmapString("Playing (Press Spacebar to Record)", grabber.getWidth() + 14, 14);
    }
    else
    {
        ofDrawBitmapString("Recording (Press Spacebar to Play)", grabber.getWidth() + 14, 14);
        ofDrawBitmapString(ofToString(frameBuffer.size()), 14, grabber.getHeight() - 28);
    }
    

    std::stringstream ss;

    ss << " App FPS: " << ofGetFrameRate() << std::endl;
    ss << " Cam FPS: " << grabber.getGrabber<ofxPS3EyeGrabber>()->getFPS() << std::endl;
    ss << "Real FPS: " << grabber.getGrabber<ofxPS3EyeGrabber>()->getActualFPS() << std::endl;
    ss << "      id: 0x" << ofToHex(grabber.getGrabber<ofxPS3EyeGrabber>()->getDeviceId());

    ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));
}

void ofApp::keyPressed(int key)
{
    auto eye = grabber.getGrabber<ofxPS3EyeGrabber>();
    
    if (key == ' ')
    {
        if (isRecording) currentFrame = 0;
        else frameBuffer.clear();
        
        isRecording = !isRecording;
    }
    else if (key == 'v' && eye)
    {
        eye->setFlipVertical(!eye->getFlipVertical());
    }
    else if (key == 'h' && eye)
    {
        eye->setFlipHorizontal(!eye->getFlipHorizontal());
    }
}
