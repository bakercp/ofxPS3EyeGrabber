ofxPS3EyeGrabber
================

A Sony PS3 Camera grabber for openFrameworks.

This class extends `ofBaseVideoGrabber` making it easy to drop in to existing projects.

This addon is largely based upon the great work of @inspirit found [here](https://github.com/inspirit/PS3EYEDriver/).

This addon requires `libusb` for direct camera access.  Rather than including that library in this addon, it is easier to simply include ofxKinect, which is a core addon and already contains the `libusb` libraries needed for this addon to work across platforms.