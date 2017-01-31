//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofApp.h"


int main()
{
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.width = 640;
	settings.height = 480;
	ofCreateWindow(settings);
	ofRunApp(std::make_shared<ofApp>());
}
