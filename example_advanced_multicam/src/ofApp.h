//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#pragma once


#include "ofMain.h"


class ofApp: public ofBaseApp
{
public:
    void setup() override;
    void update() override;
    void draw() override;

    void keyPressed(int key) override;

    std::vector<std::shared_ptr<ofVideoGrabber>> grabbers;

    int totalHeight = 0;

    int camWidth = 320;
    int camHeight = 240;
    int camFrameRate = 200;

};
