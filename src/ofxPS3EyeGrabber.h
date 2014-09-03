// =============================================================================
//
// Copyright (c) 2014 Christopher Baker <http://christopherbaker.net>
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


#include "ofThread.h"
#include "ofTypes.h"
#include "ofBaseTypes.h"
#include "ofEvents.h"
#include "ps3eye.h"


class ofxPS3EyeGrabber:
    public ofBaseVideoGrabber,
    public ofThread
{
public:
    ofxPS3EyeGrabber();

    virtual ~ofxPS3EyeGrabber();

    std::vector<ofVideoDevice> listDevices();
    bool initGrabber(int w, int h);

    void update();
    bool isFrameNew();

    unsigned char* getPixels();
    ofPixels& getPixelsRef();

    void close();

    float getHeight();
    float getWidth();

    bool setPixelFormat(ofPixelFormat pixelFormat);
    ofPixelFormat getPixelFormat();

    ofTexture* getTexture();

    void setVerbose(bool verbose);
    void setDeviceID(int deviceId);
    void setDesiredFrameRate(int framerate);
    void videoSettings();

    bool getAutogain() const;
    void setAutogain(bool val);

    bool getAutoWhiteBalance() const;
    void setAutoWhiteBalance(bool val);

    uint8_t getGain() const;
    void setGain(uint8_t val);

    uint8_t getExposure() const;
    void setExposure(uint8_t val);

    uint8_t getSharpness() const;
    void setSharpness(uint8_t val);

    uint8_t getContrast() const;
    void setContrast(uint8_t val);

    uint8_t getBrightness() const;
    void setBrightness(uint8_t val);

    uint8_t getHue() const;
    void setHue(uint8_t val);

    uint8_t getRedBalance() const;
    void setRedBalance(uint8_t val);

    uint8_t getBlueBalance() const;
    void setBlueBalance(uint8_t val);

    void setFlip(bool horizontal = false, bool vertical = false);

    float getFPS() const;

protected:
    typedef ps3eye::PS3EYECam::PS3EYERef PS3EYERef;

    static const int ITUR_BT_601_CY;
    static const int ITUR_BT_601_CUB;
    static const int ITUR_BT_601_CUG;
    static const int ITUR_BT_601_CVG;
    static const int ITUR_BT_601_CVR;
    static const int ITUR_BT_601_SHIFT;

    static void yuv422_to_rgba(const uint8_t *yuv_src,
                               const int stride,
                               uint8_t *dst,
                               const int width,
                               const int height);

    void exit(ofEventArgs& args);
    void start();
    void stop();
    void threadedFunction();

private:
    PS3EYERef _cam;
    ofPixels _pixels;

    int _deviceId;
    int _desiredFrameRate;

    bool _isFrameNew;

    float _currentFPS;
    unsigned long long _lastSampleTime;
    unsigned long long _numFramesSampled;

    enum
    {
        FPS_SAMPLE_INTERVAL = 500 // milliseconds
    };

};
