ofxPS3EyeGrabber
================

## Description

A Sony PS3 Camera grabber for openFrameworks.

## Features

This class extends `ofBaseVideoGrabber` making it easy to drop in to existing projects.

This addon is largely based upon the great work of @inspirit found [here](https://github.com/inspirit/PS3EYEDriver/).

This addon is largely intended for OSX and Windows, as Linux has a built-in driver available by default through `ofVideoGrabber`.

This addon requires `libusb` for direct camera access.  Rather than including that library in this addon, it is easier to simply include `ofxKinect`, which is a core addon and already contains the `libusb` libraries needed for this addon to work across platforms.

### Resolution and Framerate

The current drivers support the following resolutions and framerates:

|   Resolution  | Frame Rate    | Notes |
| ------------- | ------------- | ----- |
| 640x480       | 83 Hz         | _Partially corrupted frames._ |
| 640x480       | 75 Hz         |       |
| 640x480       | 60 Hz         |       |
| 640x480       | 50 Hz         |       |
| 640x480       | 40 Hz         |       |
| 640x480       | 30 Hz         |       |
| 640x480       | 25 Hz         |       |
| 640x480       | 20 Hz         |       |
| 640x480       | 15 Hz         |       |
| 640x480       | 10 Hz         |       |
| 640x480       |  8 Hz         |       |
| 640x480       |  5 Hz         |       |
| 640x480       |  3 Hz         |       |
| 640x480       |  2 Hz         |       |
| 320x240       | 290 Hz        | _Partially corrupted frames._ |
| 320x240       | 205 Hz        | _Partially corrupted frames._ |
| 320x240       | 187 Hz        |       |
| 320x240       | 150 Hz        |       |
| 320x240       | 137 Hz        |       |
| 320x240       | 125 Hz        |       |
| 320x240       | 100 Hz        |       |
| 320x240       | 75 Hz         |       |
| 320x240       | 60 Hz         |       |
| 320x240       | 50 Hz         |       |
| 320x240       | 40 Hz         |       |
| 320x240       | 37 Hz         |       |
| 320x240       | 30 Hz         |       |
| 320x240       | 17 Hz         |       |
| 320x240       | 15 Hz         |       |
| 320x240       | 12 Hz         |       |
| 320x240       | 10 Hz         |       |
| 320x240       |  7 Hz         |       |
| 320x240       |  5 Hz         |       |
| 320x240       |  3 Hz         |       |
| 320x240       |  2 Hz         |       |

Frame rates greater than noted above will default to the next slowest valid frame rate.

The number of cameras that can be used simultaneously at each framerate and resolution is limited by the 480Mbit/s data transfer rate of USB 2.0. This vaue is per USB bus, so if you have multiple USB busses (e.g. through additional PCI cards, etc), you can increase the number of cameras accordingly. In practice, it's been observed that the data rate usually maxes out around 384Mbit/s (about 80% of the theoretical maximum) which may be due to other items using the USB bus (pointing devices, keyboards, disks, etc), processing power or other factors.

### Linux

While Linux has a built-in driver for the PS3 Eye Camera, this addon offers more configuration flexibility. If you don't need that flexibility, the default `ofVideoPlayer` should give you access on Linux. If you need additional framerates, lower bandwidth, raw bayer data, etc, then this addon is for you.

This addon uses `libusb` to directly communicate with the camera.  It disables the built-in kernel driver and re-enables it upon exit. To access the USB hardware directly you need to run your application with sufficient permissions. You can do that by running your application with `sudo` (e.g. `sudo ./myApplication`) or set up a Udev rule as described below.

The PS3 Eye camera has a USB vendor id of `1415` and a USB product id of `2000`. To allow direct `libusb` access without root permissions on a Debian linux variant (e.g. Ubuntu, Raspbian, etc) add a new [udev](https://en.wikipedia.org/wiki/Udev) rule for your cameras by creating a new file

```
/etc/udev/rules.d/ps3_eye_camera.rules
```

and add the following line:

```
ATTR{idVendor}=="1415", ATTR{idProduct}=="2000", MODE+="777"
```

## Getting Started

To get started, generate the example project files using the openFrameworks [Project Generator](http://openframeworks.cc/learning/01_basics/how_to_add_addon_to_project/).

## Documentation

API documentation can be found here.

## Build Status

Linux, macOS [![Build Status](https://travis-ci.org/bakercp/ofxPS3EyeGrabber.svg?branch=master)](https://travis-ci.org/bakercp/ofxPS3EyeGrabber)

Visual Studio, MSYS [![Build status](https://ci.appveyor.com/api/projects/status/udrkwyv7jngy7m2k/branch/master?svg=true)](https://ci.appveyor.com/project/bakercp/ofxps3eyegrabber/branch/master)

## Compatibility

### Branches

The `stable` branch of this repository is meant to be compatible with the openFrameworks [stable branch](https://github.com/openframeworks/openFrameworks/tree/stable), which corresponds to the latest official openFrameworks release.

The `master` branch of this repository is meant to be compatible with the openFrameworks [master branch](https://github.com/openframeworks/openFrameworks/tree/master).

Some past openFrameworks releases are supported via tagged versions, but only `stable` and `master` branches are actively supported.

### Requirements

- ofxKinect
  - This addon depends on the core ofxKinect addon because it is the easiest way to get accces to libusb in `openFrameworks`.
- ofxOpenCv
  - This addon depends on the core ofxOpenCv addon because it uses optimized OpenCv code for converting between colorspaces (e.g. Bayer to RGB).

## Versioning

This project uses [Semantic Versioning](http://semver.org/), although strict adherence will only come into effect at version 1.0.0.

## Licensing

See `LICENSE.md`.

## Contributing

Pull Requests are always welcome, so if you make any improvements please feel free to float them back upstream :)

1. Fork this repository.
2. Create your feature branch (`git checkout -b my-new-feature`).
3. Commit your changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin my-new-feature`).
5. Create new Pull Request.
