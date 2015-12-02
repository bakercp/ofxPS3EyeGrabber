// =============================================================================
//
// Copyright (c) 2014-2015 Christopher Baker <http://christopherbaker.net>
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


#pragma once


#include "ofBaseTypes.h"
#include "ofTypes.h"
#include "ofEvents.h"
#include "ofThread.h"
#include "ps3eye.h"


class ofxPS3EyeGrabber:
    public ofBaseVideoGrabber,
    public ofThread
{
public:
    /// \brief Create an uninitialized ofxPS3EyeGrabber.
    ofxPS3EyeGrabber();

    /// \brief Destroy the PS3EyeGrabber.
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
    bool setPixelFormat(ofPixelFormat pixelFormat) override;
    ofPixelFormat getPixelFormat() const override;
    void setVerbose(bool verbose) override;
    void setDeviceID(int deviceId) override;
    void setDesiredFrameRate(int framerate) override;
    void videoSettings() override;

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

    /// \brief Flip the camera's image.
    /// \param enable true for a vertical flip.
    void setVerticalFlip(bool enable);

    /// \brief Flip the camera's image.
    /// \param enable true for a horizontal flip.
	void setHorizontalFlip(bool enable);

    /// \brief Enable a test pattern overlay.
    /// \param enable true for a test pattern.
	void setTestPattern(bool enable);

	/// \brief Enable the LED.
	/// \param enable True if the LED should be enabled.
	void setLED(bool enable);

    /// \returns the camera's current FPS value.
    float getFPS() const;

	/// \returns the camera's current actual FPS value.
	float getActualFPS() const;

protected:
    /// \brief Constant used for YUV conversion.
    static const int ITUR_BT_601_CY;

    /// \brief Constant used for YUV conversion.
    static const int ITUR_BT_601_CUB;

    /// \brief Constant used for YUV conversion.
    static const int ITUR_BT_601_CUG;

    /// \brief Constant used for YUV conversion.
    static const int ITUR_BT_601_CVG;

    /// \brief Constant used for YUV conversion.
    static const int ITUR_BT_601_CVR;

    /// \brief Constant used for YUV conversion.
    static const int ITUR_BT_601_SHIFT;

    /// \brief Convert YUV422 to RGBA
    /// \param yuv_source a pointer to the YUV data source.
    /// \param strid the YUV data stride.
    /// \param destination the destination RGBA pixels.
    /// \param width the pixel width.
    /// \param height the pixel height.
    static void yuv422_to_rgba(const uint8_t* yuv_source,
                               const int stride,
                               uint8_t* destination,
                               const int width,
                               const int height);

    void exit(ofEventArgs& args);
    void start();
    void stop();
    void threadedFunction() override;

private:
	std::shared_ptr<ps3eye::PS3EYECam> _cam;
    ofPixels _pixels;

	std::size_t _deviceId;
    int _desiredFrameRate;

    bool _isFrameNew;

	ofPixelFormat _pixelFormat;

    enum
    {
        /// \brief The default FPS sample interval in milliseconds.
        FPS_SAMPLE_INTERVAL = 500
    };

};
