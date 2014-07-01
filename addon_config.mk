meta:
	ADDON_NAME = ofxPS3EyeGrabber
	ADDON_DESCRIPTION = A Sony PS3 Camera grabber for openFrameworks.
	ADDON_AUTHOR = bakercp 
	ADDON_TAGS = "ps3eye" "camera" "ps3"
	ADDON_URL = http://github.com/bakercp/ofxPS3EyeGrabber
common:
	# We depend on ofxKinect for libusb so we don't have to package it with this addon.
	ADDON_DEPENDENCIES = ofxKinect
