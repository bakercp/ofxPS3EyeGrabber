// =============================================================================
//
// Copyright (c) 2014-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofxPS3EyeGrabber.h"
#include "ofConstants.h"


const int ofxPS3EyeGrabber::ITUR_BT_601_CY = 1220542;
const int ofxPS3EyeGrabber::ITUR_BT_601_CUB = 2116026;
const int ofxPS3EyeGrabber::ITUR_BT_601_CUG = -409993;
const int ofxPS3EyeGrabber::ITUR_BT_601_CVG = -852492;
const int ofxPS3EyeGrabber::ITUR_BT_601_CVR = 1673527;
const int ofxPS3EyeGrabber::ITUR_BT_601_SHIFT = 20;


void ofxPS3EyeGrabber::yuv422_to_rgba8888(const uint8_t* yuv_src,
                                          const int stride,
                                          uint8_t* dst,
                                          const int width,
                                          const int height)
{
    const int bIdx = 2;
    const int uIdx = 0;
    const int yIdx = 0;

    const int uidx = 1 - yIdx + uIdx * 2;
    const int vidx = (2 + uidx) % 4;

#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)

    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        uint8_t* row = dst + (width * 4) * j; // 4 channels

        for (int i = 0; i < 2 * width; i += 4, row += 8)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;

            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;
            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);
            row[3]      = (0xff);

            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[6-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[5]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[4+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
            row[7]      = (0xff);
        }
    }

#undef _max
#undef _saturate

}


void ofxPS3EyeGrabber::yuv422_to_rgb888(const uint8_t* yuv_src,
                                        const int stride,
                                        uint8_t* dst,
                                        const int width,
                                        const int height)
{
    const int bIdx = 2;
    const int uIdx = 0;
    const int yIdx = 0;

    const int uidx = 1 - yIdx + uIdx * 2;
    const int vidx = (2 + uidx) % 4;

#define _max(a, b) (((a) > (b)) ? (a) : (b))
#define _saturate(v) static_cast<uint8_t>(static_cast<uint32_t>(v) <= 0xff ? v : v > 0 ? 0xff : 0)

    for (int j = 0; j < height; j++, yuv_src += stride)
    {
        uint8_t* row = dst + (width * 3) * j; // 3 channels

        for (int i = 0; i < 2 * width; i += 4, row += 6)
        {
            int u = static_cast<int>(yuv_src[i + uidx]) - 128;
            int v = static_cast<int>(yuv_src[i + vidx]) - 128;


            int ruv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVR * v;
            int guv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CVG * v + ITUR_BT_601_CUG * u;
            int buv = (1 << (ITUR_BT_601_SHIFT - 1)) + ITUR_BT_601_CUB * u;

            int y00 = _max(0, static_cast<int>(yuv_src[i + yIdx]) - 16) * ITUR_BT_601_CY;

            row[2-bIdx] = _saturate((y00 + ruv) >> ITUR_BT_601_SHIFT);
            row[1]      = _saturate((y00 + guv) >> ITUR_BT_601_SHIFT);
            row[bIdx]   = _saturate((y00 + buv) >> ITUR_BT_601_SHIFT);

            int y01 = _max(0, static_cast<int>(yuv_src[i + yIdx + 2]) - 16) * ITUR_BT_601_CY;
            row[5-bIdx] = _saturate((y01 + ruv) >> ITUR_BT_601_SHIFT);
            row[4]      = _saturate((y01 + guv) >> ITUR_BT_601_SHIFT);
            row[3+bIdx] = _saturate((y01 + buv) >> ITUR_BT_601_SHIFT);
        }
    }

#undef _max
#undef _saturate

}


ofxPS3EyeGrabber::ofxPS3EyeGrabber(int requestedDeviceId): _requestedDeviceId(requestedDeviceId)
{
    ofAddListener(ofEvents().exit, this, &ofxPS3EyeGrabber::exit);
}


ofxPS3EyeGrabber::~ofxPS3EyeGrabber()
{
    ofRemoveListener(ofEvents().exit, this, &ofxPS3EyeGrabber::exit);
    stop();
    _cam.reset();
}


void ofxPS3EyeGrabber::exit(ofEventArgs& args)
{
    stop();
}


void ofxPS3EyeGrabber::start()
{
    if (_cam != nullptr)
    {
        _cam->start();
        startThread();
    }
}


void ofxPS3EyeGrabber::stop()
{
    stopThread();

    if (_cam != nullptr)
    {
        _cam->stop();
    }
}


void ofxPS3EyeGrabber::threadedFunction()
{
    while (isThreadRunning())
    {
        if (_cam != nullptr)
        {
            bool res = _cam->updateDevices();

            if (!res)
            {
                break;
            }
        }
    }
}


int ofxPS3EyeGrabber::getDeviceId() const
{
    if (_cam != nullptr)
    {
        return static_cast<int>(_cam->id());
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getDeviceId") << "Camera is not initialized, requested id is " << _requestedDeviceId;
        return _requestedDeviceId;
    }
}


std::vector<ofVideoDevice> ofxPS3EyeGrabber::listDevices() const
{
    std::vector<ofVideoDevice> devices;

    for (const auto& camera: ps3eye::PS3EYECam::getDevices())
    {
        ofVideoDevice device;
        device.id = camera->id(); // This is the USB Location ID.
        device.deviceName = "PS3-Eye";
        device.hardwareName = std::to_string(camera->sensorId()) + "-" + std::to_string(camera->manufacturerId()); // This is not provided by the PS3 Eye camera.
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
            if (_requestedDeviceId == AUTO_CAMERA_ID || _requestedDeviceId == device->id())
            {
                _cam = device;

                bool success = _cam->init(w, h, _requestedFrameRate);

                if (success)
                {
                    start();
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
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setup") << "Camera is already initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::update()
{
    _isFrameNew = false;

    if (_cam != nullptr)
    {
        if (_cam->isNewFrame())
        {
            if (_pixelFormat == OF_PIXELS_RGB)
            {
                if (!_pixels.isAllocated() || _pixels.getPixelFormat() != OF_PIXELS_RGB)
                {
                    _pixels.allocate(_cam->getWidth(),
                                     _cam->getHeight(),
                                     OF_PIXELS_RGB);
                }

                yuv422_to_rgb888(_cam->getLastFramePointer(),
                                 _cam->getRowBytes(),
                                 _pixels.getData(),
                                 _cam->getWidth(),
                                 _cam->getHeight());
            }
            else if (_pixelFormat == OF_PIXELS_RGBA)
            {
                if (!_pixels.isAllocated() || _pixels.getPixelFormat() != OF_PIXELS_RGBA)
                {
                    _pixels.allocate(_cam->getWidth(),
                                     _cam->getHeight(),
                                     OF_PIXELS_RGBA);
                }

                yuv422_to_rgba8888(_cam->getLastFramePointer(),
                                   _cam->getRowBytes(),
                                   _pixels.getData(),
                                   _cam->getWidth(),
                                   _cam->getHeight());
            }
            else
            {
                // Set from native pixels requires no extra copying or
                // colorspace conversion.
                _pixels.setFromExternalPixels(_cam->getLastFramePointer(),
                                              _cam->getWidth(),
                                              _cam->getHeight(),
                                              OF_PIXELS_YUY2);
            }

            _isFrameNew = true;

        }
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
    stopThread();
}


float ofxPS3EyeGrabber::getHeight() const
{
    if (_cam != nullptr)
    {
        return _cam->getHeight();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getHeight") << "Camera is not initialized.";
        return 0;
    }
}


float ofxPS3EyeGrabber::getWidth() const
{
    if (_cam != nullptr)
    {
        return _cam->getWidth();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getWidth") << "Camera is not initialized.";
        return 0;
    }
}


bool ofxPS3EyeGrabber::setPixelFormat(ofPixelFormat pixelFormat)
{
    if (pixelFormat == OF_PIXELS_NATIVE)
    {
        _pixelFormat = OF_PIXELS_YUY2;
        return true;
    }
    else if (pixelFormat == OF_PIXELS_RGB
          || pixelFormat == OF_PIXELS_RGBA
          || pixelFormat == OF_PIXELS_YUY2)
    {
        _pixelFormat = pixelFormat;
        return true;
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setPixelFormat") << "setPixelFormat(): requested pixel format " << pixelFormat << " not supported";
        return false;
    }
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
    _requestedDeviceId = deviceId;
}


void ofxPS3EyeGrabber::setDesiredFrameRate(int framerate)
{
    _requestedFrameRate = framerate;
}


void ofxPS3EyeGrabber::videoSettings()
{
    ofLogWarning("ofxPS3EyeGrabber::setDeviceID") << "Not implemented.  Call settings (e.g. setAutogain()) directly.";
}


bool ofxPS3EyeGrabber::getAutogain() const
{
    if (_cam != nullptr)
    {
        return _cam->getAutogain();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getAutogain") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setAutogain(bool val)
{
    if (_cam != nullptr)
    {
        _cam->setAutogain(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setAutogain") << "Camera is not initialized.";
    }
}


bool ofxPS3EyeGrabber::getAutoWhiteBalance() const
{
    if (_cam != nullptr)
    {
        return _cam->getAutoWhiteBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getAutoWhiteBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setAutoWhiteBalance(bool val)
{
    if (_cam != nullptr)
    {
        _cam->setAutoWhiteBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setAutoWhiteBalance") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getGain() const
{
    if (_cam != nullptr)
    {
        return _cam->getGain();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getGain") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setGain(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setGain(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setGain") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getExposure() const
{
    if (_cam != nullptr)
    {
        return _cam->getExposure();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getExposure") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setExposure(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setExposure(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setExposure") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getSharpness() const
{
    if (_cam != nullptr)
    {
        return _cam->getSharpness();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getSharpness") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setSharpness(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setSharpness(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setSharpness") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getContrast() const
{
    if (_cam != nullptr)
    {
        return _cam->getContrast();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getContrast") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setContrast(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setContrast(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setContrast") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getBrightness() const
{
    if (_cam != nullptr)
    {
        return _cam->getBrightness();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getBrightness") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setBrightness(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setBrightness(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setBrightness") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getHue() const
{
    if (_cam != nullptr)
    {
        return _cam->getHue();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getHue") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setHue(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setHue(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setHue") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getRedBalance() const
{
    if (_cam != nullptr)
    {
        return _cam->getRedBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getRedBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setRedBalance(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setRedBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setRedBalance") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getBlueBalance() const
{
    if (_cam != nullptr)
    {
        return _cam->getBlueBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getBlueBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setBlueBalance(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setBlueBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setBlueBalance") << "Camera is not initialized.";
    }
}


uint8_t ofxPS3EyeGrabber::getGreenBalance() const
{
    if (_cam != nullptr)
    {
        return _cam->getGreenBalance();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getGreenBalance") << "Camera is not initialized.";
        return false;
    }
}


void ofxPS3EyeGrabber::setGreenBalance(uint8_t val)
{
    if (_cam != nullptr)
    {
        _cam->setGreenBalance(val);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setGreenBalance") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setFlipVertical(bool enable)
{
    if (_cam != nullptr)
    {
        _cam->setVerticalFlip(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setFlipVertical") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setVerticalFlip(bool enable)
{
    setFlipVertical(enable);
}


void ofxPS3EyeGrabber::setFlipHorizontal(bool enable)
{
    if (_cam != nullptr)
    {
        _cam->setHorizontalFlip(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setFlipHorizontal") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setHorizontalFlip(bool enable)
{
    setFlipHorizontal(enable);
}


void ofxPS3EyeGrabber::setTestPattern(bool enable)
{
    if (_cam != nullptr)
    {
        _cam->setTestPattern(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setTestPattern") << "Camera is not initialized.";
    }
}


void ofxPS3EyeGrabber::setLED(bool enable)
{
    if (_cam != nullptr)
    {
        _cam->setLED(enable);
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::setFlip") << "Camera is not initialized.";
    }
}


float ofxPS3EyeGrabber::getFPS() const
{
    if (_cam != nullptr)
    {
        return _cam->getFrameRate();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getFPS") << "Camera is not initialized.";
        return 0;
    }
}


float ofxPS3EyeGrabber::getActualFPS() const
{
    if (_cam != nullptr)
    {
        return _cam->getActualFrameRate();
    }
    else
    {
        ofLogWarning("ofxPS3EyeGrabber::getActualFPS") << "Camera is not initialized.";
        return 0;
    }
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
            else if (str.compare("OF_PIXELS_RGB") == 0) format = OF_PIXELS_RGB;
            else if (str.compare("OF_PIXELS_RGBA") == 0) format = OF_PIXELS_RGBA;
            else if (str.compare("OF_PIXELS_YUY2") == 0) format = OF_PIXELS_YUY2;
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
