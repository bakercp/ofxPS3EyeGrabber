#pragma once


#include <memory>
#include <vector>
#include "libusb.h"
#include "ps3eye.h"


class USBMgr
{
public:
	USBMgr();
	~USBMgr();

	static std::shared_ptr<USBMgr> instance();
	static libusb_context* usbContext() { return instance()->usb_context; }
	static int listDevices(std::vector<ps3eye::PS3EYECam::PS3EYERef>& list);
	static bool handleEvents();

	static std::shared_ptr<USBMgr>  sInstance;
	static int sTotalDevices;

private:
	USBMgr(const USBMgr&);
	void operator=(const USBMgr&);

	libusb_context* usb_context;

};

