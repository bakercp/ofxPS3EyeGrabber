//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include "ofMain.h"
#include "ofxPS3EyeGrabber.h"


class ofApp: public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);

    std::vector<std::shared_ptr<ofVideoGrabber>> grabbers;

    int totalHeight = 0;

    int camWidth = 640;
    int camHeight = 480;
    int camFrameRate = 30;

};
