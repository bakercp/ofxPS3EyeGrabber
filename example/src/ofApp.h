//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include "ofMain.h"
#include "ps3eye.h"


class Camera
{
public:
    
    ps3eye::PS3EYECam::PS3EYERef eye;
    
    ofTexture tex;
    ofPixels pix;
    
    int camFrameCount;
    int camFpsLastSampleFrame;
    float camFpsLastSampleTime;
    float camFps;

};


class ofApp: public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    std::vector<Camera> cameras;

};
