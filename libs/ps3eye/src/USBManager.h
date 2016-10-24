// From: https://github.com/inspirit/PS3EYEDriver/

#pragma once


#include <memory>
#include <vector>
#include <cstring>
#include "libusb.h"
#include "ps3eye.h"


/// \brief A URB is a USB request block.
///
/// \sa http://www.makelinux.net/ldd3/chp-13-sect-3
class URBDesc
{
public:
    static std::vector<std::shared_ptr<ps3eye::PS3EYECam>> listDevices();

    // Values for bmHeaderInfo (Video and Still Image Payload Headers, 2.4.3.3)
    enum UVC_headers
    {
        UVC_STREAM_EOH = (1 << 7),
        UVC_STREAM_ERR = (1 << 6),
        UVC_STREAM_STI = (1 << 5),
        UVC_STREAM_RES = (1 << 4),
        UVC_STREAM_SCR = (1 << 3),
        UVC_STREAM_PTS = (1 << 2),
        UVC_STREAM_EOF = (1 << 1),
        UVC_STREAM_FID = (1 << 0),
    };

    // packet types when moving from iso buf to frame buf
    enum gspca_packet_type
    {
        DISCARD_PACKET,
        FIRST_PACKET,
        INTER_PACKET,
        LAST_PACKET
    };

    URBDesc();
    ~URBDesc();


    bool handleEvents();

    bool start_transfers(libusb_device_handle *handle, uint32_t curr_frame_size);

    void close_transfers();

    void frame_add(gspca_packet_type packet_type, const uint8_t *data, std::size_t len);

    void pkt_scan(uint8_t *data, std::size_t len);

    static void cb_xfr(struct libusb_transfer *xfr);

    // look for an input transfer endpoint in an alternate setting
    // libusb_endpoint_descriptor
    static uint8_t find_endpoint(struct libusb_device *device);

    uint8_t num_transfers = 0;

    gspca_packet_type last_packet_type;

    uint32_t last_pts = 0;
    uint16_t last_fid = 0;

    libusb_transfer* xfr[2];

    uint8_t* frame_buffer = nullptr;
    uint8_t* frame_buffer_end = nullptr;
    uint8_t* frame_data_start = nullptr;
    std::size_t frame_data_len = 0;
    uint32_t frame_size = 0;
    uint8_t frame_complete_ind = 0;
    uint8_t frame_work_ind = 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time;

    std::chrono::duration<double> frameDuration;
    
    double smoothFrameDuration = -1;

};
