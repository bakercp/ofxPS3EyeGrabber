// =============================================================================
//
// Copyright (c) 2014 Christopher Baker <http://christopherbaker.net>
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
    ofSetVerticalSync(false);

	int camWidth = 640;
	int camHeight = 480;
    int camFrameRate = 30;

    // We can now get back a list of devices.
    std::vector<ofVideoDevice> devices = ofxPS3EyeGrabber().listDevices();

	for (std::size_t i = 0; i < devices.size(); ++i)
    {
        std::stringstream ss;

        ss << devices[i].id << ": " << devices[i].deviceName;

        if (!devices[i].bAvailable)
        {
            ss << " - unavailable ";
        }
        else
        {
            std::shared_ptr<ofxPS3EyeGrabber> videoGrabber = std::make_shared<ofxPS3EyeGrabber>();

            videoGrabber->setDeviceID(i);
            videoGrabber->setDesiredFrameRate(camFrameRate);
            videoGrabber->setup(camWidth, camHeight);
            videoGrabber->setAutogain(true);
            videoGrabber->setAutoWhiteBalance(true);

            videoGrabbers.push_back(videoGrabber);

            // Add a texture.
            videoTextures.push_back(ofTexture());
        }

        ofLogNotice("ofApp::setup") << ss.str();
	}


}


void ofApp::update()
{
    for (std::size_t i = 0; i < videoGrabbers.size(); ++i)
    {
        videoGrabbers[i]->update();

        if (videoGrabbers[i]->isFrameNew())
        {
            videoTextures[i].loadData(videoGrabbers[i]->getPixels());
        }
    }
}

void ofApp::draw()
{
    ofBackground(0);
    ofSetColor(255);

    int x = 0;
    int y = 0;

    for (std::size_t i = 0; i < videoGrabbers.size(); ++i)
    {
        ofPushMatrix();
        ofTranslate(x, y);

        videoTextures[i].draw(0, 0);

        std::stringstream ss;

        ss << "App FPS: " << ofGetFrameRate() << std::endl;
        ss << "Cam FPS: " << videoGrabbers[i]->getFPS();

        ofDrawBitmapStringHighlight(ss.str(), ofPoint(10, 15));

        ofPopMatrix();

        if (x + videoGrabbers[i]->getWidth() >= ofGetWidth())
        {
            y += videoGrabbers[i]->getHeight();
            x = 0;
        }
        else
        {
            x += videoGrabbers[i]->getWidth();
        }
    }
}
