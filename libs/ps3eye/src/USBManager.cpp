#include "USBManager.h"


std::shared_ptr<USBMgr> USBMgr::sInstance;
int                     USBMgr::sTotalDevices = 0;


USBMgr::USBMgr()
{
	libusb_init(&usb_context);
	libusb_set_debug(usb_context, 1);
}

USBMgr::~USBMgr()
{
	debug("USBMgr destructor\n");
	libusb_exit(usb_context);
}

std::shared_ptr<USBMgr> USBMgr::instance()
{
	if( !sInstance ) {
		sInstance = std::make_shared<USBMgr>();
	}
	return sInstance;
}

bool USBMgr::handleEvents()
{
	return (libusb_handle_events(instance()->usb_context) == 0);
}

int USBMgr::listDevices( std::vector<ps3eye::PS3EYECam::PS3EYERef>& list )
{
	libusb_device *dev;
	libusb_device **devs;
	int i = 0;

	int cnt = libusb_get_device_list(instance()->usb_context, &devs);

	if (cnt < 0)
		debug("Error Device scan\n");

	cnt = 0;
	while ((dev = devs[i++]) != NULL)
	{
		struct libusb_device_descriptor desc;
		libusb_get_device_descriptor(dev, &desc);
		if(desc.idVendor == ps3eye::PS3EYECam::VENDOR_ID && desc.idProduct == ps3eye::PS3EYECam::PRODUCT_ID)
		{
			list.push_back( ps3eye::PS3EYECam::PS3EYERef( new ps3eye::PS3EYECam(dev) ) );
			libusb_ref_device(dev);
			cnt++;
		}
	}

	libusb_free_device_list(devs, 1);

	return cnt;
}