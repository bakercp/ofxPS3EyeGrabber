// From: https://github.com/inspirit/PS3EYEDriver/

#include "USBManager.h"
#include <iostream>


bool URBDesc::handleEvents()
{
    return (libusb_handle_events(nullptr) == 0);
}


std::vector<std::shared_ptr<ps3eye::PS3EYECam>> URBDesc::listDevices()
{
    std::vector<std::shared_ptr<ps3eye::PS3EYECam>> devices;

    libusb_init(nullptr);

    libusb_device *dev;
    libusb_device **devs;

    ssize_t cnt = libusb_get_device_list(nullptr, &devs);

    if (cnt < 0)
    {
        debug("Error Device scan\n");
    }

    cnt = 0;

    int i = 0;

    while ((dev = devs[i++]) != nullptr)
    {
        struct libusb_device_descriptor desc;

        libusb_get_device_descriptor(dev, &desc);

        if(desc.idVendor == ps3eye::PS3EYECam::VENDOR_ID &&
           desc.idProduct == ps3eye::PS3EYECam::PRODUCT_ID)
        {
            devices.push_back(std::make_shared<ps3eye::PS3EYECam>(dev));
            libusb_ref_device(dev);
            cnt++;
        }
    }

    libusb_free_device_list(devs, 1);

    libusb_exit(nullptr);

    return devices;
}



URBDesc::URBDesc():
    num_transfers(0),
    last_packet_type(DISCARD_PACKET),
    last_pts(0),
    last_fid(0)
{
    libusb_init(nullptr);
    libusb_set_debug(nullptr, LIBUSB_LOG_LEVEL_ERROR);

    // we allocate max possible size
    // 16 frames

    std::size_t stride = 640 * 2;
    const std::size_t fsz = stride * 480;

    frame_buffer = new uint8_t[fsz * 16 + 16384 * 2];

    frame_buffer_end = frame_buffer + fsz * 16;

    frame_data_start = frame_buffer;
    frame_data_len = 0;
    frame_complete_ind = 0;
    frame_work_ind = 0;
    frame_size = fsz;
}


URBDesc::~URBDesc()
{
    debug("URBDesc destructor\n");

    if (num_transfers)
    {
        close_transfers();
    }

    if (frame_buffer != nullptr)
    {
        delete [] frame_buffer;
        frame_buffer = nullptr;
    }

    debug("URBDesc destructor\n");
    libusb_exit(nullptr);
}


bool URBDesc::start_transfers(libusb_device_handle *handle, uint32_t curr_frame_size)
{
    const std::size_t bsize = 16384;

    frame_size = curr_frame_size;

    uint8_t* buff = frame_buffer_end;
    uint8_t* buff1 = buff + bsize;

    std::memset(frame_buffer_end, 0, bsize * 2);

    xfr[0] = libusb_alloc_transfer(0);
    xfr[1] = libusb_alloc_transfer(0);

    uint8_t ep_addr = find_ep(libusb_get_device(handle));

    libusb_clear_halt(handle, ep_addr);

    libusb_fill_bulk_transfer(xfr[0],
                              handle,
                              ep_addr,
                              buff,
                              bsize,
                              cb_xfr,
                              reinterpret_cast<void*>(this),
                              0);

    libusb_fill_bulk_transfer(xfr[1],
                              handle,
                              ep_addr,
                              buff1,
                              bsize,
                              cb_xfr,
                              reinterpret_cast<void*>(this),
                              0);

    int res = LIBUSB_SUCCESS;

    res |= libusb_submit_transfer(xfr[0]);
    res |= libusb_submit_transfer(xfr[1]);

    num_transfers = 2;
    frame_complete_ind = 0;
    frame_work_ind = 0;
    last_pts = 0;
    last_fid = 0;
    last_frame_time = std::chrono::time_point<std::chrono::high_resolution_clock>();

    return res == LIBUSB_SUCCESS;
}


void URBDesc::close_transfers()
{
    libusb_cancel_transfer(xfr[0]);
    libusb_cancel_transfer(xfr[1]);

    while (num_transfers)
    {
        if (!handleEvents())
        {
            break;
        }
    }
}


void URBDesc::frame_add(enum gspca_packet_type packet_type,
                        const uint8_t *data,
                        std::size_t len)
{
    if (packet_type == FIRST_PACKET)
    {
        frame_data_start = frame_buffer + frame_work_ind * frame_size;
        frame_data_len = 0;
    }
    else
    {
        switch(last_packet_type)
        {
            case DISCARD_PACKET:
                if (packet_type == LAST_PACKET)
                {
                    last_packet_type = packet_type;
                    frame_data_len = 0;
                }
                return;
            case LAST_PACKET:
                return;
            default:
                break;
        }
    }

    // Append the packet to the frame buffer.
    if (len > 0)
    {
        if (frame_data_len + len > frame_size)
        {
            packet_type = DISCARD_PACKET;
            frame_data_len = 0;
        }
        else
        {
            std::memcpy(frame_data_start + frame_data_len, data, len);
            frame_data_len += len;
        }
    }

    last_packet_type = packet_type;

    if (packet_type == LAST_PACKET)
    {

        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        frameDuration = now - last_frame_time;

//        if (0 > smoothFrameDuration)
//        {
//            smoothFrameDuration = frameDuration.count();
//        }
//        else
//        {
            smoothFrameDuration = frameDuration.count() * 0.5 + smoothFrameDuration * 0.5;
//        }


        last_frame_time = now;
        frame_complete_ind = frame_work_ind;
        frame_work_ind = (frame_work_ind + 1) & 15;
        frame_data_len = 0;
        //debug("frame completed %d\n", frame_complete_ind);
    }
}

void URBDesc::pkt_scan(uint8_t *data, std::size_t len)
{
    uint32_t this_pts;
    uint16_t this_fid;
    std::size_t remaining_len = len;

    const std::size_t payload_len = 2048; // bulk type

    do
    {
        len = std::min(remaining_len, payload_len);

        // Payloads are prefixed with a UVC-style header.  We
        // consider a frame to start when the FID toggles, or the PTS
        // changes.  A frame ends when EOF is set, and we've received
        // the correct number of bytes.

        // Verify UVC header.  Header length is always 12
        if (data[0] != 12 || len < 12)
        {
            debug("bad header\n");
            goto discard;
        }

        // Check errors
        if (data[1] & UVC_STREAM_ERR)
        {
            debug("payload error\n");
            goto discard;
        }

        // Extract PTS and FID
        if (!(data[1] & UVC_STREAM_PTS))
        {
            debug("PTS not present\n");
            goto discard;
        }

        // Read 32 bit int PTS
        this_pts = (data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2];

        // Read the FID
        this_fid = (data[1] & UVC_STREAM_FID) ? 1 : 0;

        // If PTS or FID has changed, start a new frame.
        if (this_pts != last_pts || this_fid != last_fid)
        {
            if (last_packet_type == INTER_PACKET)
            {
                frame_add(LAST_PACKET, 0, 0);
            }

            last_pts = this_pts;
            last_fid = this_fid;

            // Add the frame, minus the UVC header.
            frame_add(FIRST_PACKET, data + 12, len - 12);

        } // If this packet is marked as EOF, end the frame
        else if (data[1] & UVC_STREAM_EOF)
        {
            last_pts = 0;

            if (frame_data_len + len - 12 != frame_size)
            {
                goto discard;
            }

            frame_add(LAST_PACKET, data + 12, len - 12);
        }
        else
        {
            // Add the data from this payload
            frame_add(INTER_PACKET, data + 12, len - 12);
        }

        // Done this payload
        goto scan_next;

    discard:
        // Discard data until a new frame starts.
        frame_add(DISCARD_PACKET, 0, 0);

    scan_next:
        remaining_len -= len;
        data += len;

    }
    while (remaining_len > 0);
}



void URBDesc::cb_xfr(struct libusb_transfer* xfr)
{
    URBDesc* urb = reinterpret_cast<URBDesc*>(xfr->user_data);

    enum libusb_transfer_status status = xfr->status;

    if (status != LIBUSB_TRANSFER_COMPLETED)
    {
        debug("transfer status %d\n", status);

        libusb_free_transfer(xfr);

        urb->num_transfers--;

        if (status != LIBUSB_TRANSFER_CANCELLED)
        {
            urb->close_transfers();
        }

        return;
    }

    //debug("length:%u, actual_length:%u\n", xfr->length, xfr->actual_length);

    urb->pkt_scan(xfr->buffer, xfr->actual_length);

    if (libusb_submit_transfer(xfr) < 0)
    {
        debug("error re-submitting URB\n");
        urb->close_transfers();
    }
}


uint8_t URBDesc::find_ep(struct libusb_device* device)
{
    const struct libusb_interface_descriptor* altsetting = nullptr;
    const struct libusb_endpoint_descriptor* ep  = nullptr;
    struct libusb_config_descriptor* config = nullptr;
    uint8_t ep_addr = 0;

    libusb_get_active_config_descriptor(device, &config);

    if (!config)
    {
        return 0;
    }

    for (uint8_t i = 0; i < config->bNumInterfaces; i++)
    {
        altsetting = config->interface[i].altsetting;

        if (altsetting[0].bInterfaceNumber == 0)
        {
            break;
        }
    }

    if (altsetting != nullptr)
    {
        for (uint8_t i = 0; i < altsetting->bNumEndpoints; i++)
        {
            ep = &altsetting->endpoint[i];

            if ((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK
                && ep->wMaxPacketSize != 0)
            {
                ep_addr = ep->bEndpointAddress;
                break;
            }
        }
    }

    libusb_free_config_descriptor(config);

    return ep_addr;
}
