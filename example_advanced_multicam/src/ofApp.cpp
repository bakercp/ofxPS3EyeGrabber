// =============================================================================
//
// Copyright (c) 2014-2016 Christopher Baker <http://christopherbaker.net>
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

	int camWidth = 320;
	int camHeight = 240;
    int camFrameRate = 15;

    // We can get a list of devices.
    std::vector<ofVideoDevice> devices = ofxPS3EyeGrabber().listDevices();

    // Now cycle through the devices and set up grabbers for each.
	for (std::size_t i = 0; i < devices.size(); ++i)
    {
        std::stringstream ss;

        ss << devices[i].id << ": " << devices[i].deviceName << " : " << devices[i].serialID;

        if (!devices[i].bAvailable)
        {
            ss << " - unavailable ";
        }
        else
        {
			std::shared_ptr<ofVideoGrabber> grabber = std::make_shared<ofVideoGrabber>();

			grabber->setGrabber(std::make_shared<ofxPS3EyeGrabber>());
			grabber->setDeviceID(i);
			grabber->setDesiredFrameRate(camFrameRate);

            // The native pixel format for the ofxPS3EyeGrabber is OF_PIXELS_YUY2
            // (aka YUV422).  When used this way, no additional pixel copies are made
            // or colorspace conversions are performed.
            //
            // The programmable renderer is able to directly render YUV422 pixels.
            // so be sure to that the OpenGL version is > 3.2, otherwise you'll
            // get a blank screen.
			grabber->setPixelFormat(OF_PIXELS_NATIVE);
			grabber->setup(camWidth, camHeight);

            // Make ofxPS3EyeGrabber-specific settings updates.
			grabber->getGrabber<ofxPS3EyeGrabber>()->setAutogain(true);
			grabber->getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(true);

            grabbers.push_back(grabber);
        }

        ofLogNotice("ofApp::setup") << ss.str();
	}
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

    int x = 0;
    int y = 0;

	for (auto& g: grabbers)
	{
        ofPushMatrix();
        ofTranslate(x, y);

		g->draw(0, 0);

        std::stringstream ss;

        ss << " App FPS: " << ofGetFrameRate() << std::endl;
		ss << " Cam FPS: " << g->getGrabber<ofxPS3EyeGrabber>()->getFPS() << std::endl;
		ss << "Real FPS: " << g->getGrabber<ofxPS3EyeGrabber>()->getActualFPS();

        ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));

        ofPopMatrix();

		x += g->getWidth();

		if (x + g->getWidth() >= ofGetWidth())
		{
			y += g->getHeight();
			x = 0;
		}
    }
}

void ofApp::keyPressed(int key)
{
	if (key == 'h')
	{
		for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setHorizontalFlip(true);
	}
	else if (key == 'H')
	{
		for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setHorizontalFlip(false);
	}
	else if (key == 'v')
	{
		for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setVerticalFlip(true);
	}
	else if (key == 'V')
	{
		for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setVerticalFlip(false);
	}
	else if (key == 't')
	{
		for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setTestPattern(true);
	}
	else if (key == 'T')
	{
		for (auto& g: grabbers) g->getGrabber<ofxPS3EyeGrabber>()->setTestPattern(false);
	}
}
