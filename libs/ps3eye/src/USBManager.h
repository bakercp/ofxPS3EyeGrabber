// From: https://github.com/inspirit/PS3EYEDriver/

#pragma once


#include <memory>
#include <vector>
#include <cstring>
#include "libusb.h"
#include "ps3eye.h"


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
    static uint8_t find_ep(struct libusb_device *device);

    uint8_t num_transfers;

    gspca_packet_type last_packet_type;

    uint32_t last_pts;
    uint16_t last_fid;

    libusb_transfer* xfr[2];

    uint8_t* frame_buffer;
    uint8_t* frame_buffer_end;
    uint8_t* frame_data_start;
    std::size_t frame_data_len;
    uint32_t frame_size;
    uint8_t frame_complete_ind;
    uint8_t frame_work_ind;

    std::chrono::time_point<std::chrono::high_resolution_clock> last_frame_time;

    std::chrono::duration<double> frameDuration;
    double smoothFrameDuration = -1;

};
