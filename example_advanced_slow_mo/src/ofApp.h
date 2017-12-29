//
// Copyright (c) 2018 Christopher Baker <https://christopherbaker.net>
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

    /// \brief The video grabber.
    ofVideoGrabber grabber;

    /// \brief True if we are recording into the buffer.
    bool isRecording = false;
    
    /// \brief The current frame in the buffer.
    std::size_t currentFrame = 0;

    /// \brief The buffer of images.
    std::vector<ofTexture> frameBuffer;

};
