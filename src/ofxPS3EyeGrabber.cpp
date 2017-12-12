//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofxPS3EyeGrabber.h"
#include "ofConstants.h"


ofxPS3EyeGrabber::ofxPS3EyeGrabber(int requestedDeviceId):
    _exitListener(ofEvents().exit.newListener(this, &ofxPS3EyeGrabber::exit)),
    _requestedDeviceId(requestedDeviceId),
    _isThreadRunning(false)
{
}


ofxPS3EyeGrabber::~ofxPS3EyeGrabber()
{
  //  std::cout << "ofxPS3EyeGrabber::~ofxPS3EyeGrabber" << std::endl;

    stop();
    _cam.reset();
    
 //   std::cout << "ofxPS3EyeGrabber::~ofxPS3EyeGrabber: FINISHED" << std::endl;

}


void ofxPS3EyeGrabber::exit(ofEventArgs& args)
{
 //   std::cout << "ofxPS3EyeGrabber::exit" << std::endl;
    stop();
  //  std::cout << "ofxPS3EyeGrabber::exit done!" << std::endl;
}


void ofxPS3EyeGrabber::start()
{
    if (_cam)
        _cam->start();
}


void ofxPS3EyeGrabber::stop()
{
    if (_isThreadRunning)
    {
        _isThreadRunning = false;
        
        try
        {
            _thread.join();
        }
        catch (const std::exception& exc)
        {
            ofLogWarning("ofxPS3EyeGrabber::stop") << "Thread join failed: " << exc.what();
        }
    }
}


int ofxPS3EyeGrabber::getDeviceId() const
{
    if (_cam) return _getLocationIdForDevice(_cam->device_);

    ofLogWarning("ofxPS3EyeGrabber::getDeviceId") << "Camera is not initialized, requested id is " << _requestedDeviceId;
    return _requestedDeviceId;
}


std::vector<ofVideoDevice> ofxPS3EyeGrabber::listDevices() const
{
    std::vector<ofVideoDevice> devices;

    for (const auto& camera: ps3eye::PS3EYECam::getDevices())
    {
        ofVideoDevice device;
        device.id = _getLocationIdForDevice(camera->device_);//    0;//camera->id(); // This is the USB Location ID.
        device.deviceName = "PS3-Eye";
        device.hardwareName = "N/A";//std::to_string(camera->sensorId()) + "-" + std::to_string(camera->manufacturerId()); // This is not provided by the PS3 Eye camera.
        device.serialID = "N/A"; // This is not provided by the PS3 Eye camera.

        // device.formats.push_back(...) // TODO: Listing all of these may not be helpful.
        device.bAvailable = !camera->isStreaming();;
        devices.push_back(device);
    }

    return devices;
}

bool ofxPS3EyeGrabber::setup(int w, int h)
{
    if (_cam == nullptr)
    {
        const auto& eyeDevices = ps3eye::PS3EYECam::getDevices();

        for (const auto& device : eyeDevices)
        {
            if (_requestedDeviceId == AUTO_CAMERA_ID || _requestedDeviceId == _getLocationIdForDevice(device->device_))
            {
                _cam = device;
                
                ps3eye::PS3EYECam::EOutputFormat out;
                
                if (_pixelFormat == OF_PIXELS_RGB)
                {
                    out = ps3eye::PS3EYECam::EOutputFormat::RGB;
                }
                else if (_pixelFormat == OF_PIXELS_BGR)
                {
                    out = ps3eye::PS3EYECam::EOutputFormat::BGR;
                }
                else if (_pixelFormat == OF_PIXELS_GRAY)
                {
                    out = ps3eye::PS3EYECam::EOutputFormat::Gray;
                }
                else
                {
                    ofLogError("ofxPS3EyeGrabber::setup") << "Invalid pixel type: " << ofToString(_pixelFormat);
                    return false;
                }
                
                bool success = _cam->init(w, h, _requestedFrameRate, out);//ps3eye::PS3EYECam::EOutputFormat::Bayer);
                
                if (success)
                {
                    _pixels.allocate(_cam->getWidth(),
                                     _cam->getHeight(),
                                     _pixelFormat);
                    
                    start();

                    _isThreadRunning = true;
                    _thread = std::thread(&ofxPS3EyeGrabber::_threadedFunction, this);
                    
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }

        ofLogWarning("ofxPS3EyeGrabber::setup") << "Device id was is not found: " << "0x" << ofToHex(_requestedDeviceId);
        return false;
    }

    ofLogWarning("ofxPS3EyeGrabber::setup") << "Camera is already initialized.";
    return false;
}


void ofxPS3EyeGrabber::update()
{
    _isFrameNew = false;

    while (!_pixelChannel.empty())
    {
        _isFrameNew = true;
        _pixelChannel.receive(_pixels);
    }
}


bool ofxPS3EyeGrabber::isFrameNew() const
{
    return _isFrameNew;
}


bool ofxPS3EyeGrabber::isInitialized() const
{
    return _cam != nullptr;
}


ofPixels& ofxPS3EyeGrabber::getPixels()
{
    return _pixels;
}


const ofPixels& ofxPS3EyeGrabber::getPixels() const
{
    return _pixels;
}


void ofxPS3EyeGrabber::close()
{
    stop();
}


float ofxPS3EyeGrabber::getHeight() const
{
    if (_cam)
        return _cam->getHeight();

    ofLogWarning("ofxPS3EyeGrabber::getHeight") << "Camera is not initialized.";
    return 0;
}


float ofxPS3EyeGrabber::getWidth() const
{
    if (_cam) return _cam->getWidth();

    ofLogWarning("ofxPS3EyeGrabber::getWidth") << "Camera is not initialized.";
    return 0;
}


bool ofxPS3EyeGrabber::setPixelFormat(ofPixelFormat pixelFormat)
{
    if (pixelFormat == OF_PIXELS_NATIVE)
    {
        _pixelFormat = OF_PIXELS_RGB;
        return true;
    }
    else if (pixelFormat == OF_PIXELS_RGB
          || pixelFormat == OF_PIXELS_BGR
          || pixelFormat == OF_PIXELS_GRAY)
    {
        _pixelFormat = pixelFormat;
        return true;
    }

    
    ofLogWarning("ofxPS3EyeGrabber::setPixelFormat") << "setPixelFormat(): requested pixel format " << pixelFormat << " not supported";
    return false;
}


ofPixelFormat ofxPS3EyeGrabber::getPixelFormat() const
{
    return _pixelFormat;
}


void ofxPS3EyeGrabber::setVerbose(bool verbose)
{
    ofLogWarning("ofxPS3EyeGrabber::setVerbose") << "Not implemented.";
}


void ofxPS3EyeGrabber::setDeviceID(int deviceId)
{
    if (_cam)
        ofLogWarning("ofxPS3EyeGrabber::setDeviceID") << "Camera already initialized. Change will take place when camera is reinitialized.";

    _requestedDeviceId = deviceId;
}


void ofxPS3EyeGrabber::setDesiredFrameRate(int framerate)
{
    if (_cam)
        ofLogWarning("ofxPS3EyeGrabber::setDesiredFrameRate") << "Camera already initialized. Change will take place when camera is reinitialized.";

    _requestedFrameRate = framerate;
}


void ofxPS3EyeGrabber::videoSettings()
{
    ofLogWarning("ofxPS3EyeGrabber::setDeviceID") << "Not implemented.  Call settings (e.g. setAutogain()) directly.";
}


bool ofxPS3EyeGrabber::getAutogain() const
{
    if (_cam)
        return _cam->getAutogain();
 
    ofLogWarning("ofxPS3EyeGrabber::getAutogain") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setAutogain(bool val)
{
    if (_cam)
        _cam->setAutogain(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setAutogain") << "Camera is not initialized.";
}


bool ofxPS3EyeGrabber::getAutoWhiteBalance() const
{
    if (_cam)
        return _cam->getAutoWhiteBalance();

    ofLogWarning("ofxPS3EyeGrabber::getAutoWhiteBalance") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setAutoWhiteBalance(bool val)
{
    if (_cam)
        _cam->setAutoWhiteBalance(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setAutoWhiteBalance") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getGain() const
{
    if (_cam)
        return _cam->getGain();

    ofLogWarning("ofxPS3EyeGrabber::getGain") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setGain(uint8_t val)
{
    if (_cam)
        _cam->setGain(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setGain") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getExposure() const
{
    if (_cam)
        return _cam->getExposure();

    ofLogWarning("ofxPS3EyeGrabber::getExposure") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setExposure(uint8_t val)
{
    if (_cam)
        _cam->setExposure(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setExposure") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getSharpness() const
{
    if (_cam)
        return _cam->getSharpness();

    ofLogWarning("ofxPS3EyeGrabber::getSharpness") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setSharpness(uint8_t val)
{
    if (_cam)
        _cam->setSharpness(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setSharpness") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getContrast() const
{
    if (_cam)
        return _cam->getContrast();
    
    ofLogWarning("ofxPS3EyeGrabber::getContrast") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setContrast(uint8_t val)
{
    if (_cam)
        _cam->setContrast(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setContrast") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getBrightness() const
{
    if (_cam)
        return _cam->getBrightness();

    ofLogWarning("ofxPS3EyeGrabber::getBrightness") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setBrightness(uint8_t val)
{
    if (_cam)
        _cam->setBrightness(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setBrightness") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getHue() const
{
    if (_cam)
        return _cam->getHue();

    ofLogWarning("ofxPS3EyeGrabber::getHue") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setHue(uint8_t val)
{
    if (_cam)
        _cam->setHue(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setHue") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getRedBalance() const
{
    if (_cam)
        return _cam->getRedBalance();

    ofLogWarning("ofxPS3EyeGrabber::getRedBalance") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setRedBalance(uint8_t val)
{
    if (_cam)
        _cam->setRedBalance(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setRedBalance") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getBlueBalance() const
{
    if (_cam)
        return _cam->getBlueBalance();

    ofLogWarning("ofxPS3EyeGrabber::getBlueBalance") << "Camera is not initialized.";
    return false;
}


void ofxPS3EyeGrabber::setBlueBalance(uint8_t val)
{
    if (_cam)
        _cam->setBlueBalance(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setBlueBalance") << "Camera is not initialized.";
}


uint8_t ofxPS3EyeGrabber::getGreenBalance() const
{
    if (_cam)
        return _cam->getGreenBalance();

    ofLogWarning("ofxPS3EyeGrabber::getGreenBalance") << "Camera is not initialized.";
}


void ofxPS3EyeGrabber::setGreenBalance(uint8_t val)
{
    if (_cam)
        _cam->setGreenBalance(val);
    else
        ofLogWarning("ofxPS3EyeGrabber::setGreenBalance") << "Camera is not initialized.";
}


void ofxPS3EyeGrabber::setFlipVertical(bool enable)
{
    if (_cam)
        _cam->setFlip(_cam->getFlipH(), enable);
    else
        ofLogWarning("ofxPS3EyeGrabber::setFlipVertical") << "Camera is not initialized.";
}


void ofxPS3EyeGrabber::setVerticalFlip(bool enable)
{
    setFlipVertical(enable);
}


void ofxPS3EyeGrabber::setFlipHorizontal(bool enable)
{
    if (_cam)
        _cam->setFlip(enable, _cam->getFlipV());
    else
        ofLogWarning("ofxPS3EyeGrabber::setFlipHorizontal") << "Camera is not initialized.";
}


void ofxPS3EyeGrabber::setHorizontalFlip(bool enable)
{
    setFlipHorizontal(enable);
}


void ofxPS3EyeGrabber::setTestPattern(bool enable)
{
    if (_cam)
        ;//_cam->setTestPattern(enable);
    else
        ofLogWarning("ofxPS3EyeGrabber::setTestPattern") << "Camera is not initialized.";
}


void ofxPS3EyeGrabber::setLED(bool enable)
{
    if (_cam)
        _cam->ov534_set_led(enable);
    else
        ofLogWarning("ofxPS3EyeGrabber::setFlip") << "Camera is not initialized.";
}


float ofxPS3EyeGrabber::getFPS() const
{
    if (_cam)
        return _cam->getFrameRate();

    ofLogWarning("ofxPS3EyeGrabber::getFPS") << "Camera is not initialized.";
    return 0;
}


float ofxPS3EyeGrabber::getActualFPS() const
{
    if (_cam)
        return 0;//_cam->getActualFrameRate();

    ofLogWarning("ofxPS3EyeGrabber::getActualFPS") << "Camera is not initialized.";
    return 0;
}


std::shared_ptr<ofVideoGrabber> ofxPS3EyeGrabber::fromJSON(const ofJson& json)
{
    auto grabber = std::make_shared<ofVideoGrabber>();
    grabber->setGrabber(std::make_shared<ofxPS3EyeGrabber>());

    int width = -1;
    int height = -1;
    bool isUsingTexture = false;

    auto iter = json.cbegin();
    while (iter != json.cend())
    {
        const auto& key = iter.key();
        const auto& value = iter.value();

        if (key == "id" && !value.is_null())
        {
            const auto& str = value.get<std::string>();

            if (str == "auto") grabber->setDeviceID(ofxPS3EyeGrabber::AUTO_CAMERA_ID);
            else if (str.substr(0, 2).compare("0x") == 0) grabber->setDeviceID(ofHexToInt(str));
            else grabber->setDeviceID(ofToInt(str));
        }

        else if (key == "frame_rate") grabber->setDesiredFrameRate(value);
        else if (key == "width") width = value;
        else if (key == "height") height = value;
        else if (key == "use_texture") isUsingTexture = value;
        else if (key == "pixel_format" && !value.is_null())
        {
            const auto& str = value.get<std::string>();
            ofPixelFormat format = OF_PIXELS_RGB;
            if (str.compare("OF_PIXELS_NATIVE") == 0) format = OF_PIXELS_NATIVE;
            else if (str.compare("OF_PIXELS_GRAY") == 0) format = OF_PIXELS_GRAY;
            else if (str.compare("OF_PIXELS_RGB") == 0) format = OF_PIXELS_RGB;
            else if (str.compare("OF_PIXELS_BGR") == 0) format = OF_PIXELS_BGR;
            else ofLogError() << "Unknown pixel_type: " << str;
            grabber->setPixelFormat(format);
        }
        else if (key == "grabber")
        {
            // skip until after setup.
        }
        else ofLogWarning("ofxPS3EyeGrabber::fromJSON") << "Unknown key: " << key;

        ++iter;
    }

    if (width == -1) width == ofxPS3EyeGrabber::DEFAULT_WIDTH;
    if (height == -1) height == ofxPS3EyeGrabber::DEFAULT_HEIGHT;

    if (!grabber->setup(width, height, isUsingTexture))
    {
        ofLogWarning("ofxPS3EyeGrabber::fromJSON") << "Setup failed.";
    }
    else
    {
        auto iter = json.cbegin();
        while (iter != json.cend())
        {
            const auto& key = iter.key();
            const auto& value = iter.value();

            if (key == "grabber")
            {
                auto iter_grabber = value.cbegin();
                while (iter_grabber != value.cend())
                {
                    const auto& key_grabber = iter_grabber.key();
                    const auto& value_grabber = iter_grabber.value();

                    auto grabber_grabber = grabber->getGrabber<ofxPS3EyeGrabber>();

                    if (key_grabber == "type") { /* not used */ }
                    else if (key_grabber == "auto_gain") grabber_grabber->setAutogain(value_grabber);
                    else if (key_grabber == "auto_white_balance") grabber_grabber->setAutoWhiteBalance(value_grabber);
                    else if (key_grabber == "gain") grabber_grabber->setGain(value_grabber);
                    else if (key_grabber == "exposure") grabber_grabber->setExposure(value_grabber);
                    else if (key_grabber == "sharpness") grabber_grabber->setSharpness(value_grabber);
                    else if (key_grabber == "hue") grabber_grabber->setHue(value_grabber);
                    else if (key_grabber == "brightness") grabber_grabber->setBrightness(value_grabber);
                    else if (key_grabber == "contrast") grabber_grabber->setContrast(value_grabber);
                    else if (key_grabber == "red_balance") grabber_grabber->setRedBalance(value_grabber);
                    else if (key_grabber == "green_balance") grabber_grabber->setGreenBalance(value_grabber);
                    else if (key_grabber == "blue_balance") grabber_grabber->setBlueBalance(value_grabber);
                    else if (key_grabber == "flip_horizontal") grabber_grabber->setFlipHorizontal(value_grabber);
                    else if (key_grabber == "flip_vertical") grabber_grabber->setFlipVertical(value_grabber);
                    else if (key_grabber == "test_pattern") grabber_grabber->setTestPattern(value_grabber);
                    else if (key_grabber == "enable_led") grabber_grabber->setLED(value_grabber);
                    else ofLogWarning("ofxPS3EyeGrabber::fromJSON") << "Unknown grabber key: " << key_grabber;
                    ++iter_grabber;
                }
            }
            
            ++iter;
        }
    }

    return grabber;
}


void ofxPS3EyeGrabber::_threadedFunction()
{
//    ofPixels pixels;
//    pixels.allocate(_cam->getWidth(), _cam->getHeight(), _pixelFormat);

    while (_isThreadRunning)
    {
            ofPixels pixels;
            pixels.allocate(_cam->getWidth(), _cam->getHeight(), _pixelFormat);
        
//        std::cout << ofToHex(getDeviceId()) << ": ofxPS3EyeGrabber::_threadedFunction: " << pixels.size() << std::endl;
        
     //   std::cout << ofToHex(getDeviceId()) << ": ofxPS3EyeGrabber::_threadedFunction: getFrame" << std::endl;
        _cam->getFrame(pixels.getData());
//        std::cout << ofToHex(getDeviceId()) << ": ofxPS3EyeGrabber::_threadedFunction: getFrame (done)" << std::endl;

        _pixelChannel.send(pixels);

//        std::cout << ofToHex(getDeviceId()) << ": ofxPS3EyeGrabber::_threadedFunction: finish loop" << std::endl;

//        if (!_isThreadRunning)
//        {
//            std::cout << ofToHex(getDeviceId()) << ": ofxPS3EyeGrabber::stopping camera" << std::endl;
//            _cam->stop();
//            
//        }

    }

    _cam->stop();

 //   std::cout <<ofToHex(getDeviceId()) << ":  THReAD FINISHED" << std::endl;

}



int ofxPS3EyeGrabber::_getLocationIdForDevice(libusb_device* device)
{
    // As per the USB 3.0 specs, the current maximum limit for the depth is 7.
    uint8_t portNumbers[8];
    
    int numPorts = libusb_get_port_numbers(device, portNumbers, 8);
    
    if (numPorts != LIBUSB_ERROR_OVERFLOW)
    {
        int _id = 0;
        
        _id |= (libusb_get_bus_number(device) << 24);
        
        for (int i = 0; i < numPorts; ++i)
        {
            _id |= (portNumbers[i] << (20 - 4 * i));
        }
        
        return _id;
    }
    
    const char* pErrorString = libusb_error_name(numPorts);
    std::cerr << (pErrorString ? std::string(pErrorString) : "Error getting location id for device.") << std::endl;
    
    return -1;
}



