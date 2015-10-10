#include "URBDesc.h"
#include "USBManager.h"



URBDesc::URBDesc():
num_transfers(0),
last_packet_type(DISCARD_PACKET),
last_pts(0),
last_fid(0)
{
	// we allocate max possible size
	// 16 frames
	size_t stride = 640*2;
	const size_t fsz = stride*480;
	frame_buffer = (uint8_t*)std::malloc(fsz * 16 + 16384*2);
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
	if(num_transfers)
		close_transfers();

	if(frame_buffer != NULL)
		free(frame_buffer);

	frame_buffer = NULL;
}


bool URBDesc::start_transfers(libusb_device_handle *handle, uint32_t curr_frame_size)
{
	struct libusb_transfer *xfr0,*xfr1;
	uint8_t* buff, *buff1;
	uint8_t ep_addr;
	std::size_t bsize = 16384;

	frame_size = curr_frame_size;

	// bulk transfers
	xfr0 = libusb_alloc_transfer(0);
	xfr1 = libusb_alloc_transfer(0);

	buff = frame_buffer_end;
	buff1 = buff + bsize;
	std::memset(frame_buffer_end, 0, bsize*2);

	xfr[0] = xfr0;
	xfr[1] = xfr1;

	ep_addr = find_ep(libusb_get_device(handle));
	//debug("found ep: %d\n", ep_addr);

	libusb_clear_halt(handle, ep_addr);

	libusb_fill_bulk_transfer(xfr0, handle, ep_addr, buff, bsize, cb_xfr, reinterpret_cast<void*>(this), 0);
	libusb_fill_bulk_transfer(xfr1, handle, ep_addr, buff1, bsize, cb_xfr, reinterpret_cast<void*>(this), 0);

	int res = libusb_submit_transfer(xfr0);
	res |= libusb_submit_transfer(xfr1);

	num_transfers = 2;
	frame_complete_ind = 0;
	frame_work_ind = 0;
	last_pts = 0;
	last_fid = 0;
	last_frame_time = std::chrono::time_point<std::chrono::high_resolution_clock>();

	return res == 0;
}

void URBDesc::close_transfers()
{
	libusb_cancel_transfer(xfr[0]);
	libusb_cancel_transfer(xfr[1]);

	while (num_transfers)
	{
		if (!USBMgr::instance()->handleEvents())
		{
			break;
		}
	}
}

void URBDesc::frame_add(enum gspca_packet_type packet_type, const uint8_t *data, std::size_t len)
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

	/* append the packet to the frame buffer */

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
		last_frame_time = std::chrono::high_resolution_clock::now();
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
	std::size_t payload_len;

	payload_len = 2048; // bulk type

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

		this_pts = (data[5] << 24) | (data[4] << 16) | (data[3] << 8) | data[2];

		this_fid = (data[1] & UVC_STREAM_FID) ? 1 : 0;

		// If PTS or FID has changed, start a new frame.
		if (this_pts != last_pts || this_fid != last_fid)
		{
			if (last_packet_type == INTER_PACKET)
			{
				frame_add(LAST_PACKET, NULL, 0);
			}

			last_pts = this_pts;
			last_fid = this_fid;

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
		frame_add(DISCARD_PACKET, NULL, 0);

	scan_next:
		remaining_len -= len;
		data += len;

	} while (remaining_len > 0);
}



void URBDesc::cb_xfr(struct libusb_transfer *xfr)
{
	URBDesc *urb = reinterpret_cast<URBDesc*>(xfr->user_data);
	enum libusb_transfer_status status = xfr->status;

	if (status != LIBUSB_TRANSFER_COMPLETED)
	{
		debug("transfer status %d\n", status);

		libusb_free_transfer(xfr);
		urb->num_transfers--;

		if(status != LIBUSB_TRANSFER_CANCELLED)
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


uint8_t URBDesc::find_ep(struct libusb_device *device)
{
	const struct libusb_interface_descriptor *altsetting;
	const struct libusb_endpoint_descriptor *ep;
	struct libusb_config_descriptor *config;
	uint8_t ep_addr = 0;

	libusb_get_active_config_descriptor(device, &config);

	if (!config)
		return 0;

	for (uint8_t i = 0; i < config->bNumInterfaces; i++)
	{
		altsetting = config->interface[i].altsetting;

		if (altsetting[0].bInterfaceNumber == 0) {
			break;
		}
	}

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

	libusb_free_config_descriptor(config);

	return ep_addr;
}
