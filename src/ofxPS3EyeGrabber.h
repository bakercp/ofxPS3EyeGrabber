//
// Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#pragma once


#include <vector>
#include "ofJson.h"
#include "ofVideoBaseTypes.h"
#include "ofTypes.h"
#include "ofEvents.h"
#include "ofThreadChannel.h"
#include "ofVideoGrabber.h"
#include "ps3eye.h"


/// \brief Create an ofxPS3EyeGrabber.
class ofxPS3EyeGrabber: public ofBaseVideoGrabber
{
public:
    /// \brief A class for a frame which includes the pixels and a timestamp in microseconds.
    struct Frame
    {
        ofPixels pixels;
        uint64_t timestamp = 0;
    };
    

    /// \brief A type describing the demosaicing algorithm used with native capture pixels.
    enum class DemosaicType
    {
        /// \brief Default bilinear interpolation.
        DEMOSAIC_BILINEAR,
        /// \brief Demosaicing using Variable Number of Gradients.
        DEMOSAIC_VNG
    };
    
    /// \brief Create an uninitialized ofxPS3EyeGrabber.
    /// \param requestedDeviceId The requested device id.
    ofxPS3EyeGrabber(std::size_t requestedDeviceId = AUTO_CAMERA_ID);

    /// \brief Destroy the ofxPS3EyeGrabber.
    virtual ~ofxPS3EyeGrabber();

    std::vector<ofVideoDevice> listDevices() const override;
    bool setup(int w, int h) override;
    void update() override;
    bool isFrameNew() const override;
    bool isInitialized() const override;
    ofPixels& getPixels() override;
    const ofPixels& getPixels() const override;
    void close() override;
    float getHeight() const override;
    float getWidth() const override;

    /// \brief Set the requested ofPixelFormat.
    ///
    /// The grabber can output the following pixel ofPixelFormats:
    ///
    ///     OF_PIXELS_RGB (default)
    ///     OF_PIXELS_BGR
    ///     OF_PIXELS_GRAY
    ///     OF_PIXELS_NATIVE
    ///
    /// OF_PIXELS_RGB, OF_PIXELS_BGR, OF_PIXELS_RGB and OF_PIXELS_GRAY will
    /// transfer pixels from the camera as an 8-bit / pixel Bayer pattern and
    /// will be converted to the requested color pixel type.
    ///
    /// OF_PIXELS_NATIVE defaults to OF_PIXELS_GRAY and the results are an
    /// unconverted 8-bit per pixel Bayer pattern.
    ///
    /// \param pixelFormat the requested ofPixelFormat.
    /// \returns true if the format was successfully changed.
    bool setPixelFormat(ofPixelFormat pixelFormat) override;
    ofPixelFormat getPixelFormat() const override;
    void setVerbose(bool verbose) override;
    void setDeviceID(int deviceId) override;
    void setDesiredFrameRate(int framerate) override;
    void videoSettings() override;

    /// \returns all new frames.
    std::vector<Frame> getAllFrames() const;

    std::size_t getDeviceId() const;

    /// \returns true iff auto gain is enabled.
    bool getAutogain() const;

    /// \brief Set the auto gain.
    /// \param val the auto gain value.
    void setAutogain(bool val);

    /// \returns true iff auto white balance is enabled.
    bool getAutoWhiteBalance() const;

    /// \brief Set the auto white balance.
    /// \param val the auto white balance value.
    void setAutoWhiteBalance(bool val);

    /// \returns the current gain setting.
    uint8_t getGain() const;

    /// \brief Set the camera's gain.
    /// \param val a gain between 0-63.
    void setGain(uint8_t val);

    /// \returns the current exposure setting.
    uint8_t getExposure() const;

    /// \brief Set the camera's exposure.
    /// \param val an exposure between 0-255.
    void setExposure(uint8_t val);

    /// \returns the current sharpness setting.
    /// \param val a sharpness between 0-63.
    uint8_t getSharpness() const;

    /// \brief Set the camera's sharpness.
    void setSharpness(uint8_t val);

    /// \returns the current contrast setting.
    uint8_t getContrast() const;

    /// \brief Set the camera's contrast.
    /// \param val a contrast between 0-255.
    void setContrast(uint8_t val);

    /// \returns the current brightness setting.
    uint8_t getBrightness() const;

    /// \brief Set the camera's brightness.
    /// \param val a brightness between 0-255.
    void setBrightness(uint8_t val);

    /// \returns the current hue setting.
    uint8_t getHue() const;

    /// \brief Set the camera's hue.
    /// \param val a hue between 0-255.
    void setHue(uint8_t val);

    /// \returns the current red balance setting.
    uint8_t getRedBalance() const;

    /// \brief Set the camera's red balance.
    /// \param val a red balance between 0-255.
    void setRedBalance(uint8_t val);

    /// \returns the current blue balance setting.
    uint8_t getBlueBalance() const;

    /// \brief Set the camera's blue balance.
    /// \param val a blue balance between 0-255.
    void setBlueBalance(uint8_t val);

    /// \returns the current green balance setting.
    uint8_t getGreenBalance() const;

    /// \brief Set the camera's green balance.
    /// \param val a blue balance between 0-255.
    void setGreenBalance(uint8_t val);

    /// \returns true of the image is flipped vertically.
    bool getFlipVertical() const;
    
    /// \brief Flip the camera's image.
    /// \param enable true for a vertical flip.
    void setFlipVertical(bool enable);
    OF_DEPRECATED_MSG("Use setFlipVertical() instead", void setVerticalFlip(bool enable));

    /// \returns true of the image is flipped horizontally.
    bool getFlipHorizontal() const;
    
    /// \brief Flip the camera's image.
    /// \param enable true for a horizontal flip.
    void setFlipHorizontal(bool enable);
    OF_DEPRECATED_MSG("Use setFlipHorizontal() instead", void setHorizontalFlip(bool enable));

    /// \brief Set the demosaic algorithm.
    /// \param type The type to set.
    void setDemosaicType(DemosaicType type);
    
    /// \returns the current demosaicing algorithm.
    DemosaicType getDemosaicType() const;    
    
    /// \returns true if the test pattern is enabled.
    bool getTestPattern() const;
    
    /// \brief Enable a test pattern overlay.
    /// \param enable true for a test pattern.
    void setTestPattern(bool enable);

    /// \brief Enable the LED.
    /// \param enable True if the LED should be enabled.
    void setLED(bool enable);

    /// \returns the camera's current requested FPS value.
    float getFPS() const;

    /// \returns the camera's current actual FPS value.
    float getActualFPS() const;
    
    enum : std::size_t
    {
        /// \brief An automatic camera id will connect to the first available camera.
        AUTO_CAMERA_ID = std::numeric_limits<std::size_t>::max(),

        /// \brief The default camera width.
        DEFAULT_WIDTH = 640,

        /// \brief The default camera height.
        DEFAULT_HEIGHT = 480,
    };

    static ofPixels bayerConverter(ofPixels& bayerPixels,
                                   ofPixelFormat targetFormat,
                                   bool vFlip,
                                   DemosaicType _demosaicType);
    
    /// \brief Setup a ofxPS3EyeGrabber based ofVideoGrabber from JSON.
    ///
    /// \returns the configured and set up camera.
    static std::shared_ptr<ofVideoGrabber> fromJSON(const ofJson& json);

private:
    static std::size_t _getLocationIdForDevice(libusb_device* device);
    
    ofPixelFormat _transferPixelFormat() const;
    
    ofEventListener _exitListener;
    ofEventListener _updateListener;

    void _start();
    void _stop();

    /// \brief Callback for an exit event.
    /// \param args the event callback parameter.
    void _exit(ofEventArgs& args);
    void _update(ofEventArgs& args);

    /// \brief A shared pointer to the underlying camera device.
    std::shared_ptr<ps3eye::PS3EYECam> _cam;

    /// \brief The device id.
    std::size_t _requestedDeviceId = AUTO_CAMERA_ID;

    /// \brief The requested framerate.
    int _requestedFrameRate = 60;

    /// \brief A collection of the latest frames.
    std::vector<Frame> _frames;

    /// \brief The current display pixels.
    ofPixels _pixels;

    /// \brief The desired pixel format.
    ofPixelFormat _pixelFormat = OF_PIXELS_RGB;

    DemosaicType _demosaicType = DemosaicType::DEMOSAIC_BILINEAR;
    
    void _threadedFunction();

    std::atomic<float> _actualFrameRate;
    std::atomic<bool> _isThreadRunning;
    std::thread _thread;
    ofThreadChannel<Frame> _frameChannel;

};
