ofxPS3EyeGrabber
================

A Sony PS3 Camera grabber for openFrameworks.

This class extends `ofBaseVideoGrabber` making it easy to drop in to existing projects.

This addon is largely based upon the great work of @inspirit found [here](https://github.com/inspirit/PS3EYEDriver/).

This addon is largely intended for OSX and Windows, as Linux has a built-in driver available by default through `ofVideoGrabber`.

This addon requires `libusb` for direct camera access.  Rather than including that library in this addon, it is easier to simply include ofxKinect, which is a core addon and already contains the `libusb` libraries needed for this addon to work across platforms.

# Resolution and Framerate

The current drivers support the following resolutions and framerates:

|   Resolution  | Frame Rate    | Notes |
| ------------- | ------------- | ----- |
| 640x480       | 60 Hz         |       |
| 640x480       | 50 Hz         |       |
| 640x480       | 40 Hz         |       |
| 640x480       | 30 Hz         |       |
| 640x480       | 15 Hz         |       |
| 320x240       | 205 Hz        | _Experimental_ |
| 320x240       | 187 Hz        | _Experimental_ |
| 320x240       | 150 Hz        |       |
| 320x240       | 137 Hz        |       |
| 320x240       | 125 Hz        |       |
| 320x240       | 100 Hz        |       |
| 320x240       | 75 Hz         |       |
| 320x240       | 60 Hz         |       |
| 320x240       | 50 Hz         |       |
| 320x240       | 37 Hz         |       |
| 320x240       | 30 Hz         |       |


If you specify a resolution larger than 640 x 480, it will default to 640x480.

If you specify a resolution less than 640 x 480, it will default to 320 x 240.

Frame rates greater than noted above will default to the next slowest valid frame rate.
