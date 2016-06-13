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

The number of cameras that can be used simultaneously at each framerate and resolution is limited by the 480Mbit/s data transfer rate of USB 2.0. To calculate possible values you can use the equation: width * height * FPS * # of cameras * 8 / 1,000,000. If the resulting value is less than 480 it is theoretically possible. In practice, it's been observed that values shouldn't exceed 80% of the 480Mbit/s limit to account for bandwidth and processing power.

Linux
=====

In order to use this on Linux, the kernel driver must be disabled to avoid conflicts between the kernel driver and libusb.  One way to disable this is to issue this command after plugging in all of the devices:

```
sudo modprobe -r gspca_ov534
```

If you would like the driver to remain disabled after a reboot, you can add it to your kernels blacklist file by creating /etc/modprobe.d/blacklist.conf and adding the following line

```
blacklist gspca_ov534
```

Currently this program must be run with sudo privileges in order for libusb to interface with the usb cameras. To run it as a regular user add a new udev rule for the cameras.

First you must find out the 'idVendor' and 'idProduct' values that your computer assigns to the cameras. Plug one in and run

```
tail /var/log/messages
```
you should see somthing like this

```
Jun 10 17:18:54 scarf-end-2-0 kernel: [ 3004.677120] usb 1-1.4: new high-speed USB           device number 8 using dwc_otg
Jun 10 17:18:54 scarf-end-2-0 kernel: [ 3004.779642] usb 1-1.4: New USB device found, idVendor=1415, idProduct=2000
Jun 10 17:18:54 scarf-end-2-0 kernel: [ 3004.779665] usb 1-1.4: New USB device strings: Mfr=1, Product=2, SerialNumber=0
Jun 10 17:18:54 scarf-end-2-0 kernel: [ 3004.779678] usb 1-1.4: Product: USB Camera-B4.09.24.1
Jun 10 17:18:54 scarf-end-2-0 kernel: [ 3004.779692] usb 1-1.4: Manufacturer: OmniVision Technologies, Inc.
Jun 10 17:18:54 scarf-end-2-0 kernel: [ 3004.785507] usb 1-1.4: current rate 1259 is different from the runtime rate 16000
```

Create a new file /etc/udev/rules.d/camera.rules and add the following line, replacing my idVendor and idProduct values with yours.

```
ATTR{idVendor}=="1415", ATTR{idProduct}=="2000", MODE+="777"
```