//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"


void ofApp::setup()
{
    ofSetVerticalSync(true);

    // We can get a list of devices.
    std::vector<ofVideoDevice> devices = ofxPS3EyeGrabber().listDevices();

    // Now cycle through the devices and set up grabbers for each.
    for (std::size_t i = 0; i < devices.size(); ++i)
    {
        std::stringstream ss;

        // Since the PS3Eye does not provide a serial number via its USB
        // interface, the devices[i].id offers the next best alternative. The
        // devices[i].id is constructed from the USB topology and should remain
        // constant over reboots as long as the USB topology remains the same
        // (i.e. everything stays plugged into the same hubs, ports, etc). For
        // reference, this number should be equal to the Location ID on OSX and
        // can be found in the System Profiler.

        ss << "id: 0x" << ofToHex(devices[i].id) << " ";
        ss << "name: " << devices[i].deviceName << " ";
        ss << "hardware: " << devices[i].hardwareName << " ";
        ss << "serial: " << devices[i].serialID;

        if (!devices[i].bAvailable)
        {
            ss << " - unavailable ";
        }
        else
        {
            std::cout << "Making grabber" << std::endl;
            std::shared_ptr<ofVideoGrabber> grabber = std::make_shared<ofVideoGrabber>();

            grabber->setGrabber(std::make_shared<ofxPS3EyeGrabber>(devices[i].id));
            grabber->setDesiredFrameRate(camFrameRate);

//            grabber->setPixelFormat(OF_PIXELS_RGB);
//            grabber->setPixelFormat(OF_PIXELS_BGR);
//            grabber->setPixelFormat(OF_PIXELS_GRAY);
//            grabber->setPixelFormat(OF_PIXELS_NATIVE);
            grabber->setup(camWidth, camHeight);

            // Make ofxPS3EyeGrabber-specific settings updates.
            grabber->getGrabber<ofxPS3EyeGrabber>()->setAutogain(true);
            grabber->getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(true);

            grabbers.push_back(grabber);
            
        }

        ofLogNotice("ofApp::setup") << ss.str();
    }

    totalHeight = grabbers.size() * camHeight;
}


void ofApp::update()
{
    // Update all grabbers.
    for (auto& g: grabbers) g->update();

}


void ofApp::draw()
{
    ofBackground(0);
    ofSetColor(255);

    float yOffset = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0, totalHeight - camHeight, true);

    int x = 0;
    int y = 0;

    for (auto& g: grabbers)
    {
        ofPushMatrix();
        ofTranslate(x, y - yOffset);

        g->draw(0, 0);

        std::stringstream ss;

        ss << " App FPS: " << ofGetFrameRate() << std::endl;
        ss << " Cam FPS: " << g->getGrabber<ofxPS3EyeGrabber>()->getFPS() << std::endl;
        ss << "Real FPS: " << g->getGrabber<ofxPS3EyeGrabber>()->getActualFPS() << std::endl;
        ss << "      id: 0x" << ofToHex(g->getGrabber<ofxPS3EyeGrabber>()->getDeviceId());

        ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));

        ofPopMatrix();

        x += g->getWidth();

        if (x + g->getWidth() >= ofGetWidth())
        {
            y += g->getHeight();
            x = 0;
        }
    }

    std::stringstream ss;

    ss << "Mouse: Scroll through cameras." << std::endl;
    ss << "    h: Toggle Horizontal Flip" << std::endl;
    ss << "    v: Toggle Vertical Flip" << std::endl;
    ss << "    t: Toggle Test Pattern" << std::endl;
    ss << "  k/l: Toggle LED" << std::endl;
    ss << "    d: Cycle Demosaicing Types";

    ofDrawBitmapStringHighlight(ss.str(), ofGetWidth() - 250, 16);
}

void ofApp::keyPressed(int key)
{
    if (key == 'h')
    {
        for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setFlipHorizontal(!g->getGrabber<ofxPS3EyeGrabber>()->getFlipHorizontal());
    }
    else if (key == 'v')
    {
        for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setFlipVertical(!g->getGrabber<ofxPS3EyeGrabber>()->getFlipVertical());
    }
    else if (key == 't')
    {
        for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setTestPattern(!g->getGrabber<ofxPS3EyeGrabber>()->getTestPattern());
    }
    else if (key == 'l')
    {
        for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setLED(true);
    }
    else if (key == 'k')
    {
        for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setLED(false);
    }
    else if (key == 'd')
    {
        for (auto& g: grabbers)
        {
            ofxPS3EyeGrabber::DemosaicType type =  g->getGrabber<ofxPS3EyeGrabber>()->getDemosaicType();
            
            switch (type)
            {
                case ofxPS3EyeGrabber::DemosaicType::DEMOSAIC_BILINEAR:
                    type = ofxPS3EyeGrabber::DemosaicType::DEMOSAIC_VNG;
                    break;
                case ofxPS3EyeGrabber::DemosaicType::DEMOSAIC_VNG:
                    type = ofxPS3EyeGrabber::DemosaicType::DEMOSAIC_EA;
                    break;
                case ofxPS3EyeGrabber::DemosaicType::DEMOSAIC_EA:
                    type = ofxPS3EyeGrabber::DemosaicType::DEMOSAIC_BILINEAR;
                    break;
            }
            
            g->getGrabber<ofxPS3EyeGrabber>()->setDemosaicType(type);
            
        }
    }

}
