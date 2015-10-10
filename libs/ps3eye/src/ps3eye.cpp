// From: https://github.com/inspirit/PS3EYEDriver/

#include "ps3eye.h"
#include "USBManager.h"
#include "URBDesc.h"


#if defined WIN32 || defined _WIN32 || defined WINCE
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#include <chrono>
#include <cstring>
#if defined __MACH__ && defined __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
#endif

namespace ps3eye {

// Register list of SCCB and microcontroller.
#define OV534_REG_ADDRESS	0xF1 // MS_ID	/* sensor address */
#define OV534_REG_SUBADDR	0xF2 // MS_ADDRESS
#define OV534_REG_WRITE		0xF3 // MS_DO
#define OV534_REG_READ		0xF4 // MS_DI, read only
#define OV534_REG_OPERATION	0xF5 // MS_CTRL
#define OV534_REG_STATUS	0xF6 // MS_STATUS

#define OV534_REG_RSVD_0	0xF7 // RSVD
#define OV534_REG_RSVD_1	0xF8 // RSVD

#define OV534_REG_MC_BIST	0xF9 // MC_BIST, microcontroller built-in-self-test
#define OV534_REG_MC_AL		0xFA // MC_AL, Program memory pointer address low byte
#define OV534_REG_MC_AH		0xFB // MC_AH, Program memory pointer address high byte
#define OV534_REG_MC_D		0xFC // MC_D, Program memory pointer access address

#define OV534_REG_SAMPLE	0xFD // SAMPLE, Audio comb filter controll
#define OV534_REG_AC_BIST	0xFE // AC_BIST, Audio controller built-in-self-test

#define OV534_REG_RSVD_2    0xFF // RSVD

// Register list of system controller.
#define OV534_REG_RESET0    0xE0 // RESET0 Reset
#define OV534_REG_RESET1    0xE1 // RESET1 Clock enable

#define OV534_REG_CLOCK0    0xE2 // CLOCK0 Clock control.
#define OV534_REG_CLOCK1    0xE3 // CLOCK1 Clock control.

#define OV534_REG_RSVD_3    0xE4 // RSVD

#define OV534_REG_CAMERA_CLK 0xE5 // CAMERA_CLK control.
#define OV534_REG_USER		0xE6 // USER defined.

#define OV534_REG_SYS_CTRL	0xE7 // SYS_CTRL.

#define OV534_REG_STEP0	0xE8 // Audio clock parameter 0.
#define OV534_REG_STEP1	0xE9 // Audio clock parameter 0.
#define OV534_REG_MAX0	0xEA // Audio clock parameter 1.
#define OV534_REG_MAX1	0xEB // Audio clock parameter 1.

#define OV534_REG_IRQ_M0 0xEC // Interrupt Mask 0.
#define OV534_REG_IRQ_M1 0xED // Interrupt Mask 1.
#define OV534_REG_IRQ0 0xEE // Interrupt Status 0.
#define OV534_REG_IRQ1 0xEF // Interrupt Status 1.


#define OV534_REG_DIF 0x35 // Select, RGB Mode, compression mode, etc.
#define OV534_REG_CIF_FRAME 0x3B // CIF frame control
#define OV534_REG_IPU_FRAME 0x3C // CIF frame control

#define OV534_REG_PHY_BIST0 0x3D // BIST data 0.
#define OV534_REG_PHY_BIST1 0x3E // BIST data 1.
#define OV534_REG_PHY_BIST2 0x3F // BIST control.

// Register list of GPIO

#define OV534_REG_GPIO_N0 0x20 // GPIO Control.
#define OV534_REG_GPIO_C0 0x21 // GPIO Control.
#define OV534_REG_GPIO_I0 0x22 // GPIO Control.
#define OV534_REG_GPIO_V0 0x23 // GPIO Control.
#define OV534_REG_GPIO_N1 0x24 // GPIO Control.
#define OV534_REG_GPIO_C1 0x25 // GPIO Control.
#define OV534_REG_GPIO_I1 0x26 // GPIO Control.
#define OV534_REG_GPIO_V1 0x27 // GPIO Control.








#define OV534_OP_WRITE_3	0x37 //
#define OV534_OP_WRITE_2	0x33
#define OV534_OP_READ_2		0xf9

#define CTRL_TIMEOUT 500

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_A) (sizeof(_A) / sizeof((_A)[0]))
#endif


#define OV772X_SLAVE_ADDRESS_WRITE 0x42
#define OV772X_SLAVE_ADDRESS_READ 0x43



#define OV772X_REG_GAIN 0x00
#define OV772X_REG_BLUE 0x01
#define OV772X_REG_RED 0x02
#define OV772X_REG_GREEN 0x03
#define OV772X_REG_COM1 0x04
#define OV772X_REG_BAVG 0x05
#define OV772X_REG_GAVG 0x06
#define OV772X_REG_RAVG 0x07
#define OV772X_REG_AECH 0x07
#define OV772X_REG_COM2 0x09
#define OV772X_REG_PID 0x0A
#define OV772X_REG_VER 0x0B
#define OV772X_REG_COM3 0x0C
#define OV772X_REG_COM4 0x0D
#define OV772X_REG_COM5 0x0E
#define OV772X_REG_COM6 0x0F
#define OV772X_REG_AEC 0x10
#define OV772X_REG_CLCRC 0x11
#define OV772X_REG_COM7 0x12
#define OV772X_REG_COM8 0x13
#define OV772X_REG_COM9 0x14
#define OV772X_REG_COM10 0x15
#define OV772X_REG_RSVD0 0x16
#define OV772X_REG_HSTART 0x17
#define OV772X_REG_HSIZE 0x18
#define OV772X_REG_VSTRT 0x19
#define OV772X_REG_VSIZE 0x1A
#define OV772X_REG_PSHIFT 0x1B
#define OV772X_REG_MIDH 0x1C // Manufacturer ID Byte - Low
#define OV772X_REG_MIDL 0x1D // Manufacturer ID Byte - High
#define OV772X_REG_RSVD1 0x1E
#define OV772X_REG_LAEC 0x1F
#define OV772X_REG_COM11 0x20

#define OV772X_REG_RSVD2 0x21
#define OV772X_REG_BDBASE 0x22
#define OV772X_REG_DBSTEP 0x23
#define OV772X_REG_AEW 0x24
#define OV772X_REG_AEB 0x25
#define OV772X_REG_VPT 0x26
#define OV772X_REG_RSVD3 0x27
#define OV772X_REG_RSVD4 0x28
#define OV772X_REG_HOUTSIZE 0x29

#define OV772X_REG_EXHCH 0x2A
#define OV772X_REG_EXHCL 0x2B
#define	OV772X_REG_VOUTSIZE 0x2C

#define	OV772X_REG_ADVFL 0x2D
#define	OV772X_REG_ADVFH 0x2E
#define	OV772X_REG_YAVE 0x2F
#define	OV772X_REG_LUMHTH 0x30
#define	OV772X_REG_LUMLTH 0x31
#define	OV772X_REG_HREF 0x32
#define	OV772X_REG_DM_LNL 0x33
#define	OV772X_REG_DM_LNH 0x34
#define	OV772X_REG_ADOFF_B 0x35
#define	OV772X_REG_ADOFF_R 0x36
#define	OV772X_REG_ADOFF_GB 0x37
#define	OV772X_REG_ADOFF_GR 0x38
#define	OV772X_REG_OFF_B 0x39
#define	OV772X_REG_OFF_R 0x3A
#define	OV772X_REG_OFF_GB 0x3B
#define	OV772X_REG_OFF_GR 0x3C
#define OV772X_REG_COM12 0x3D
#define OV772X_REG_COM13 0x3E
#define OV772X_REG_COM14 0x3F
#define OV772X_REG_COM15 0x40
#define OV772X_REG_COM16 0x41

#define OV772X_REG_TGT_B 0x42
#define OV772X_REG_TGT_R 0x43
#define OV772X_REG_TGT_GB 0x44
#define OV772X_REG_TGT_GR 0x45
#define OV772X_REG_LCC0 0x46
#define OV772X_REG_LCC1 0x47
#define OV772X_REG_LCC2 0x48
#define OV772X_REG_LCC3 0x49
#define OV772X_REG_LCC4 0x4A
#define OV772X_REG_LCC5 0x4B
#define OV772X_REG_LCC6 0x4C
#define OV772X_REG_FIXGAIN 0x4D
#define OV772X_REG_AREF0 0x4E
#define OV772X_REG_AREF1 0x4F
#define OV772X_REG_AREF2 0x50
#define OV772X_REG_AREF3 0x51
#define OV772X_REG_AREF4 0x52
#define OV772X_REG_AREF5 0x53
#define OV772X_REG_AREF6 0x54
#define OV772X_REG_AREF7 0x55

#define OV772X_REG_RSVD5 0x56
#define OV772X_REG_RSVD6 0x57
#define OV772X_REG_RSVD7 0x58
#define OV772X_REG_RSVD8 0x59
#define OV772X_REG_RSVD9 0x5A
#define OV772X_REG_RSVD10 0x5B
#define OV772X_REG_RSVD11 0x5C
#define OV772X_REG_RSVD12 0x5D
#define OV772X_REG_RSVD13 0x5E
#define OV772X_REG_RSVD14 0x5F

#define OV772X_REG_UFIX 0x60
#define OV772X_REG_VFIX 0x61
#define OV772X_REG_AWBB_BLK 0x62

#define OV772X_REG_AWB_CTRL0 0x63 /// \brief Auto white balance Control Byte 0.

#define OV772X_REG_DSP_CTRL1 0x64 /// \brief DSP Control Byte 1.
#define OV772X_REG_DSP_CTRL2 0x65 /// \brief DSP Control Byte 2: Scaling control
#define OV772X_REG_DSP_CTRL3 0x66 /// \brief DSP Control Byte 3.
#define OV772X_REG_DSP_CTRL4 0x67 /// \brief DSP Control Byte 4.

#define OV772X_REG_AWB_BIAS 0x68
#define OV772X_REG_AWB_CTRL1 0x69
#define OV772X_REG_AWB_CTRL2 0x6A
#define OV772X_REG_AWB_CTRL3 0x6B
#define OV772X_REG_AWB_CTRL4 0x6C
#define OV772X_REG_AWB_CTRL5 0x6D
#define OV772X_REG_AWB_CTRL6 0x6E
#define OV772X_REG_AWB_CTRL7 0x6F
#define OV772X_REG_AWB_CTRL8 0x70
#define OV772X_REG_AWB_CTRL9 0x71
#define OV772X_REG_AWB_CTRL10 0x72
#define OV772X_REG_AWB_CTRL11 0x73
#define OV772X_REG_AWB_CTRL12 0x74
#define OV772X_REG_AWB_CTRL13 0x75
#define OV772X_REG_AWB_CTRL14 0x76
#define OV772X_REG_AWB_CTRL15 0x77
#define OV772X_REG_AWB_CTRL16 0x78
#define OV772X_REG_AWB_CTRL17 0x79
#define OV772X_REG_AWB_CTRL18 0x7A
#define OV772X_REG_AWB_CTRL19 0x7B
#define OV772X_REG_AWB_CTRL20 0x7C
#define OV772X_REG_AWB_CTRL21 0x7D

#define OV772X_REG_AWB_GAM1 0x7E
#define OV772X_REG_AWB_GAM2 0x7F
#define OV772X_REG_AWB_GAM3 0x80
#define OV772X_REG_AWB_GAM4 0x81
#define OV772X_REG_AWB_GAM5 0x82
#define OV772X_REG_AWB_GAM6 0x83
#define OV772X_REG_AWB_GAM7 0x84
#define OV772X_REG_AWB_GAM8 0x85
#define OV772X_REG_AWB_GAM9 0x86
#define OV772X_REG_AWB_GAM10 0x87
#define OV772X_REG_AWB_GAM11 0x88
#define OV772X_REG_AWB_GAM12 0x89
#define OV772X_REG_AWB_GAM13 0x8A
#define OV772X_REG_AWB_GAM14 0x8B
#define OV772X_REG_AWB_GAM15 0x8C

#define OV772X_REG_AWB_SLOP 0x8D
#define OV772X_REG_AWB_DNSTH 0x8E
#define OV772X_REG_AWB_EDGE0 0x8F
#define OV772X_REG_AWB_EDGE1 0x90
#define OV772X_REG_AWB_DNSOFF 0x91
#define OV772X_REG_AWB_EDGE2 0x92
#define OV772X_REG_AWB_EDGE3 0x93
#define OV772X_REG_AWB_MTX1 0x94
#define OV772X_REG_AWB_MTX2 0x95
#define OV772X_REG_AWB_MTX3 0x96
#define OV772X_REG_AWB_MTX4 0x97
#define OV772X_REG_AWB_MTX5 0x98
#define OV772X_REG_AWB_MTX6 0x99
#define OV772X_REG_AWB_MTX_CTRL 0x9A
#define OV772X_REG_AWB_BRIGHT 0x9B // brightness control
#define OV772X_REG_AWB_CNST 0x9C // constrast control
#define OV772X_REG_AWB_CNST_CTR 0x9D
#define OV772X_REG_AWB_UVADJ0 0x9E
#define OV772X_REG_AWB_UVADJ1 0x9F

#define OV772X_REG_AWB_SCAL0 0xA0
#define OV772X_REG_AWB_SCAL1 0xA1
#define OV772X_REG_AWB_SCAL2 0xA2
#define OV772X_REG_AWB_FIFODLYM 0xA3
#define OV772X_REG_AWB_FIFODLYA 0xA4
#define OV772X_REG_AWB_RSVD15 0xA5
#define OV772X_REG_AWB_SDE 0xA6
#define OV772X_REG_AWB_USAT 0xA7
#define OV772X_REG_AWB_VSAT 0xA8
#define OV772X_REG_AWB_HUE0 0xA9
#define OV772X_REG_AWB_HUE1 0xAA
#define OV772X_REG_AWB_SIGN 0xAB
#define OV772X_REG_AWB_DSPAUTO 0xAC


    static const uint8_t ov772x_reg_initdata[][2] = {

        {OV772X_REG_COM7, 0x80 }, // Reset all registers to the default values.
        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },
//        {OV772X_REG_CLCRC, 0x01 },

        { OV772X_REG_COM12, 0x03 },

		{ OV772X_REG_HSTART, 0x23 }, // VGA
        { OV772X_REG_HSIZE, 0xA0 }, // VGA
        { OV772X_REG_VSTRT, 0x07 }, // VGA
        { OV772X_REG_VSIZE, 0xF0 }, // VGA

        { OV772X_REG_HREF, 0x00 },
        { OV772X_REG_HOUTSIZE, 0xA0 }, // VGA
        { OV772X_REG_VOUTSIZE, 0xF0 }, // VGA
        { OV772X_REG_DSP_CTRL2, 0x20 },
        { 0x11, 0x01 },
        { 0x42, 0x7f },
        { OV772X_REG_AWB_CTRL0, 0xAA }, //
        { OV772X_REG_DSP_CTRL1, 0xff },
        { OV772X_REG_DSP_CTRL3, 0x00 },
        { OV772X_REG_COM8, 0xf0 },	// COM8  - jfrancois 0xf0	orig x0f7
        { 0x0d, 0x41 },
        { 0x0f, 0xc5 },
        { 0x14, 0x11 },



        {0x22, 0x7f },
        {0x23, 0x03 },
        {0x24, 0x40 },
        {0x25, 0x30 },
        {0x26, 0xa1 },
        {0x2a, 0x00 },
        {0x2b, 0x00 },
        {0x6b, 0xaa },
        {OV772X_REG_COM8, 0xff },	// COM8 - jfrancois 0xff orig 0xf7

        {0x90, 0x05 },
        {0x91, 0x01 },
        {0x92, 0x03 },
        {0x93, 0x00 },
        {0x94, 0x60 },
        {0x95, 0x3c },
        {0x96, 0x24 },
        {0x97, 0x1e },
        {0x98, 0x62 },
        {0x99, 0x80 },
        {0x9a, 0x1e },
        {0x9b, 0x08 },
        {0x9c, 0x20 },
        {0x9e, 0x81 },

        {0xa6, 0x04 },
        {0x7e, 0x0c },
        {0x7f, 0x16 },
        {0x80, 0x2a },
        {0x81, 0x4e },
        {0x82, 0x61 },
        {0x83, 0x6f },
        {0x84, 0x7b },
        {0x85, 0x86 },
        {0x86, 0x8e },
        {0x87, 0x97 },
        {0x88, 0xa4 },
        {0x89, 0xaf },
        {0x8a, 0xc5 },
        {0x8b, 0xd7 },
        {0x8c, 0xe8 },
        {0x8d, 0x20 },

        {0x0c, 0x90 },

        {0x2b, 0x00 },
        {0x22, 0x7f },
        {0x23, 0x03 },
        {0x11, 0x01 },
        {0x0c, 0xd0 },
        {0x64, 0xff },
        {0x0d, 0x41 },

        {0x14, 0x41 },
        {0x0e, 0xcd },
        {0xac, 0xbf },
        {0x8e, 0x00 },	// De-noise threshold - jfrancois 0x00 - orig 0x04, default 00
        {0x0c, 0xd0 }   // COM3, default 00, 0xd0 = 11010000

    };


	static const uint8_t ov534_reg_initdata[][2] = {
		{ 0xe7, 0x3a },

		{ OV534_REG_ADDRESS, OV772X_SLAVE_ADDRESS_WRITE }, /* select OV772x sensor */

		{ 0xc2, 0x0c },
		{ 0x88, 0xf8 },
		{ 0xc3, 0x69 },
		{ 0x89, 0xff },
		{ 0x76, 0x03 },
		{ 0x92, 0x01 },
		{ 0x93, 0x18 },
		{ 0x94, 0x10 },
		{ 0x95, 0x10 },
		{ 0xe2, 0x00 },
		{ 0xe7, 0x3e },

		{ 0x96, 0x00 },

		{ 0x97, 0x20 },
		{ 0x97, 0x20 },
		{ 0x97, 0x20 },
		{ 0x97, 0x0a },
		{ 0x97, 0x3f },
		{ 0x97, 0x4a },
		{ 0x97, 0x20 },
		{ 0x97, 0x15 },
		{ 0x97, 0x0b },

		{ 0x8e, 0x40 },
		{ 0x1f, 0x81 },
		{ 0x34, 0x05 },
		{ 0xe3, 0x04 },
		{ 0x88, 0x00 },
		{ 0x89, 0x00 },
		{ 0x76, 0x00 },
		{ 0xe7, 0x2e },
		{ 0x31, 0xf9 },
		{ 0x25, 0x42 },
		{ 0x21, 0xf0 },

		{ 0x1c, 0x00 },
		{ 0x1d, 0x40 },
		{ 0x1d, 0x02 }, /* payload size 0x0200 * 4 = 2048 bytes */
		{ 0x1d, 0x00 }, /* payload size */

		// -------------

		//	{ 0x1d, 0x01 },/* frame size */		// kwasy
		//	{ 0x1d, 0x4b },/* frame size */
		//	{ 0x1d, 0x00 }, /* frame size */


		//	{ 0x1d, 0x02 },/* frame size */		// macam
		//	{ 0x1d, 0x57 },/* frame size */
		//	{ 0x1d, 0xff }, /* frame size */

		{ 0x1d, 0x02 },/* frame size */		// jfrancois / linuxtv.org/hg/v4l-dvb
		{ 0x1d, 0x58 },/* frame size */
		{ 0x1d, 0x00 }, /* frame size */

		// ---------

		{ 0x1c, 0x0a },
		{ 0x1d, 0x08 }, /* turn on UVC header */
		{ 0x1d, 0x0e }, /* .. */

		{ 0x8d, 0x1c },
		{ 0x8e, 0x80 },
		{ 0xe5, 0x04 },

		// ----------------
		//	{ 0xc0, 0x28 },//	kwasy / macam
		//	{ 0xc1, 0x1e },//

		{ 0xc0, 0x50 },		// jfrancois
		{ 0xc1, 0x3c },
		{ 0xc2, 0x0c },



	};

	static const uint8_t bridge_start_vga[][2] = {
        {0x1c, 0x00},
        {0x1d, 0x40},
        {0x1d, 0x02},
        {0x1d, 0x00},
        {0x1d, 0x02},
        {0x1d, 0x58},
        {0x1d, 0x00},
        {0xc0, 0x50},
        {0xc1, 0x3c},
    };
    static const uint8_t sensor_start_vga[][2] = {
        {OV772X_REG_COM7, 0x00},
        {0x17, 0x26},
        {0x18, 0xa0},
        {0x19, 0x07},
        {0x1a, 0xf0},
        {0x29, 0xa0},
        {0x2c, 0xf0},
        {OV772X_REG_DSP_CTRL2, 0x20},
    };
    static const uint8_t bridge_start_qvga[][2] = {
        {0x1c, 0x00},
        {0x1d, 0x40},
        {0x1d, 0x02},
        {0x1d, 0x00},
        {0x1d, 0x01},
        {0x1d, 0x4b},
        {0x1d, 0x00},
        {0xc0, 0x28},
        {0xc1, 0x1e},
    };
    static const uint8_t sensor_start_qvga[][2] = {
        {OV772X_REG_COM7, 0x40},
        {0x17, 0x3f},
        {0x18, 0x50},
        {0x19, 0x03},
        {0x1a, 0x78},
        {0x29, 0x50},
        {0x2c, 0x78},
        {OV772X_REG_DSP_CTRL2, 0x2f},
    };



    const uint16_t PS3EYECam::VENDOR_ID = 0x1415;
    const uint16_t PS3EYECam::PRODUCT_ID = 0x2000;

    // PS3EYECam

    bool PS3EYECam::devicesEnumerated = false;
    std::vector<PS3EYECam::PS3EYERef> PS3EYECam::devices;

    const std::vector<PS3EYECam::PS3EYERef>& PS3EYECam::getDevices( bool forceRefresh )
    {
        if( devicesEnumerated && ( ! forceRefresh ) )
            return devices;

        devices.clear();

        USBMgr::instance()->sTotalDevices = USBMgr::instance()->listDevices(devices);

        devicesEnumerated = true;
        return devices;
    }

    bool PS3EYECam::updateDevices()
    {
        return USBMgr::instance()->handleEvents();
    }

    PS3EYECam::PS3EYECam(libusb_device *device)
    {
        // default controls
        autogain = false;
        gain = 20;
        exposure = 120;
        sharpness = 0;
        hue = 143;
        awb = false;
        brightness = 20;
        contrast =  37;
        blueblc = 128;
		redblc = 128;
		greenblc = 128;
        flip_h = false;
        flip_v = false;

        usb_buf = NULL;
        handle_ = NULL;

        is_streaming = false;

        device_ = device;
        mgrPtr = USBMgr::instance();
        urb = std::make_shared<URBDesc>();
    }

    PS3EYECam::~PS3EYECam()
    {
        stop();
        release();
    }

    void PS3EYECam::release()
    {
        if(handle_ != NULL)
            close_usb();
        if(usb_buf)
			free(usb_buf);
    }

    bool PS3EYECam::init(uint32_t width, uint32_t height, uint8_t desiredFrameRate)
    {
        uint16_t sensor_id;

        // open usb device so we can setup and go
        if(handle_ == NULL)
        {
            if( !open_usb() )
            {
                return false;
            }
        }

        //
        if(usb_buf == NULL)
            usb_buf = (uint8_t*)malloc(64);

        // find best cam mode
        if((width == 0 && height == 0) || width > 320 || height > 240)
        {
            frame_width = 640;
            frame_height = 480;
        } else {
            frame_width = 320;
            frame_height = 240;
        }
        frame_rate = ov534_set_frame_rate(desiredFrameRate, true);
        frame_stride = frame_width * 2;
        //

        /* reset bridge */
        ov534_reg_write(0xe7, 0x3a);
        ov534_reg_write(OV534_REG_RESET0, 0x08);

#if defined WIN32 || defined _WIN32 || defined WINCE
        Sleep(100);
#else
        nanosleep((const struct timespec[]){{0, 100000000L}}, NULL);
#endif

        /* initialize the sensor address */
        ov534_reg_write(OV534_REG_ADDRESS, 0x42);

        /* reset sensor */
        sccb_reg_write(OV772X_REG_COM7, 0x80);


#if defined WIN32 || defined _WIN32 || defined WINCE
        Sleep(10);
#else
        nanosleep((const struct timespec[]){{0, 10000000L}}, NULL);
#endif

        /* probe the sensor */
        sccb_reg_read(OV772X_REG_PID);
        sensor_id = sccb_reg_read(OV772X_REG_PID) << 8;

		sccb_reg_read(OV772X_REG_VER);

		sensor_id |= sccb_reg_read(OV772X_REG_VER);


        debug("Sensor ID: %04x\n", sensor_id);

        /* initialize */
        reg_w_array(ov534_reg_initdata, ARRAY_SIZE(ov534_reg_initdata));
        ov534_set_led(1);
        sccb_w_array(ov772x_reg_initdata, ARRAY_SIZE(ov772x_reg_initdata));
        ov534_reg_write(OV534_REG_RESET0, 0x09);
        ov534_set_led(0);

        return true;
    }

    void PS3EYECam::start()
    {
        if(is_streaming) return;

        if (frame_width == 320) {	/* 320x240 */
            reg_w_array(bridge_start_qvga, ARRAY_SIZE(bridge_start_qvga));
            sccb_w_array(sensor_start_qvga, ARRAY_SIZE(sensor_start_qvga));
        } else {		/* 640x480 */
            reg_w_array(bridge_start_vga, ARRAY_SIZE(bridge_start_vga));
            sccb_w_array(sensor_start_vga, ARRAY_SIZE(sensor_start_vga));
        }

        ov534_set_frame_rate(frame_rate);

        setAutogain(autogain);
        setAutoWhiteBalance(awb);
        setGain(gain);
        setHue(hue);
        setExposure(exposure);
        setBrightness(brightness);
        setContrast(contrast);
        setSharpness(sharpness);
        setRedBalance(redblc);
		setBlueBalance(blueblc);
		setGreenBalance(greenblc);
        setFlip(flip_h, flip_v);

        ov534_set_led(1);
        ov534_reg_write(OV534_REG_RESET0, 0x00); // start stream

        // init and start urb
        urb->start_transfers(handle_, frame_stride*frame_height);

		last_qued_frame_time = std::chrono::time_point<std::chrono::high_resolution_clock>();

        is_streaming = true;
    }

    void PS3EYECam::stop()
    {
        if(!is_streaming) return;

        /* stop streaming data */
        ov534_reg_write(OV534_REG_RESET0, 0x09);
        ov534_set_led(0);

        // close urb
        urb->close_transfers();

        is_streaming = false;
    }

	bool PS3EYECam::getAutogain() const
	{
		return autogain;
	}


	void PS3EYECam::setAutogain(bool val)
	{
		autogain = val;
		if (val) {
			sccb_reg_write(0x13, 0xf7); //AGC,AEC,AWB ON
			sccb_reg_write(0x64, sccb_reg_read(0x64)|0x03);
		} else {
			sccb_reg_write(0x13, 0xf0); //AGC,AEC,AWB OFF
			sccb_reg_write(0x64, sccb_reg_read(0x64)&0xFC);

			setGain(gain);
			setExposure(exposure);
		}
	}

	bool PS3EYECam::getAutoWhiteBalance() const
	{
		return awb;
	}


	void PS3EYECam::setAutoWhiteBalance(bool val)
	{
		awb = val;

		if (val)
		{
			sccb_reg_write(0x63, 0xe0); //AWB ON
		}
		else
		{
			sccb_reg_write(0x63, 0xAA); //AWB OFF
		}
	}

	uint8_t PS3EYECam::getGain() const { return gain; }


	void PS3EYECam::setGain(uint8_t val) {
		gain = val;
		switch(val & 0x30){
			case 0x00:
				val &=0x0F;
				break;
			case 0x10:
				val &=0x0F;
				val |=0x30;
				break;
			case 0x20:
				val &=0x0F;
				val |=0x70;
				break;
			case 0x30:
				val &=0x0F;
				val |=0xF0;
				break;
		}
		sccb_reg_write(0x00, val);
	}
	uint8_t PS3EYECam::getExposure() const { return exposure; }

	void PS3EYECam::setExposure(uint8_t val) {
		exposure = val;
		sccb_reg_write(0x08, val>>7);
		sccb_reg_write(0x10, val<<1);
	}

	uint8_t PS3EYECam::getSharpness() const { return sharpness; }
	void PS3EYECam::setSharpness(uint8_t val) {
		sharpness = val;
		sccb_reg_write(0x91, val); //vga noise
		sccb_reg_write(0x8E, val); //qvga noise
	}

	uint8_t PS3EYECam::getContrast() const { return contrast; }
	void PS3EYECam::setContrast(uint8_t val) {
		contrast = val;
		sccb_reg_write(0x9C, val);
	}
	uint8_t PS3EYECam::getBrightness() const { return brightness; }
	void PS3EYECam::setBrightness(uint8_t val) {
		brightness = val;
		sccb_reg_write(0x9B, val);
	}
	uint8_t PS3EYECam::getHue() const { return hue; }

	void PS3EYECam::setHue(uint8_t val) {
		hue = val;
		sccb_reg_write(0x01, val);
	}

	uint8_t PS3EYECam::getRedBalance() const { return redblc; }

	void PS3EYECam::setRedBalance(uint8_t val) {
		redblc = val;
		sccb_reg_write(0x43, val);
	}
	uint8_t PS3EYECam::getBlueBalance() const { return blueblc; }

	void PS3EYECam::setBlueBalance(uint8_t val) {
		blueblc = val;
		sccb_reg_write(0x42, val);
	}

	uint8_t PS3EYECam::getGreenBalance() const { return greenblc; }

	void PS3EYECam::setGreenBalance(uint8_t val) {
		greenblc = val;
		sccb_reg_write(0x44, val);
	}
	void PS3EYECam::setFlip(bool horizontal, bool vertical) {
		flip_h = horizontal;
		flip_v = vertical;
		uint8_t val = sccb_reg_read(0x0c);
		val &= ~0xc0;
		if (!horizontal) val |= 0x40;
		if (!vertical) val |= 0x80;
		sccb_reg_write(0x0c, val);
	}


	bool PS3EYECam::isStreaming() const { return is_streaming; }

	bool PS3EYECam::isNewFrame() const
	{
		if(last_qued_frame_time < urb->last_frame_time)
		{
			return true;
		}
		return false;
	}

	const uint8_t* PS3EYECam::getLastFramePointer()
	{
		last_qued_frame_time = urb->last_frame_time;
		const uint8_t* frame = const_cast<uint8_t*>(urb->frame_buffer + urb->frame_complete_ind * urb->frame_size);
		return frame;
	}

	uint32_t PS3EYECam::getWidth() const { return frame_width; }
	uint32_t PS3EYECam::getHeight() const { return frame_height; }
	uint8_t PS3EYECam::getFrameRate() const { return frame_rate; }
	uint32_t PS3EYECam::getRowBytes() const { return frame_stride; }

	void PS3EYECam::setLED(bool enable)
	{
		ov534_set_led(enable);
	}


    bool PS3EYECam::open_usb()
    {
        // open, set first config and claim interface
        int res = libusb_open(device_, &handle_);
        if(res != 0) {
            debug("device open error: %d\n", res);
            return false;
        }
        
        //libusb_set_configuration(handle_, 0);
        
        res = libusb_claim_interface(handle_, 0);
        if(res != 0) {
            debug("device claim interface error: %d\n", res);
            return false;
        }
        
        return true;
    }
    
    void PS3EYECam::close_usb()
    {
        debug("closing device\n");
        libusb_release_interface(handle_, 0);
        libusb_close(handle_);
        libusb_unref_device(device_);
        handle_ = NULL;
        device_ = NULL;
        debug("device closed\n");
    }
    
    /* Two bits control LED: 0x21 bit 7 and 0x23 bit 7.
     * (direction and output)? */
    void PS3EYECam::ov534_set_led(int status)
    {
		uint8_t data;
        
        debug("led status: %d\n", status);
        
        data = ov534_reg_read(OV534_REG_GPIO_C0);
        data |= 0x80;
        ov534_reg_write(OV534_REG_GPIO_C0, data);
        
        data = ov534_reg_read(OV534_REG_GPIO_V0);
        if (status)
            data |= 0x80;
        else
            data &= ~0x80;
        
        ov534_reg_write(OV534_REG_GPIO_V0, data);
        
        if (!status) {
            data = ov534_reg_read(OV534_REG_GPIO_C0);
            data &= ~0x80;
            ov534_reg_write(OV534_REG_GPIO_C0, data);
        }
    }
    
    /* validate frame rate and (if not dry run) set it */
    uint8_t PS3EYECam::ov534_set_frame_rate(uint8_t frame_rate, bool dry_run)
    {
        int i;
        struct rate_s {
            uint8_t fps; // Human readable fps.
            uint8_t r11; // Register 0x11 value. Register CLKRC, Internal Clock.
            uint8_t r0d; // Register 0x0D value. Register COM4, PLL control and AEC Evaluate window.
            uint8_t re5; // Register 0xE5 value. ?
        };

		const struct rate_s *r;

        static const struct rate_s rate_0[] = { /* 640x480 */
            {60, 0x01, 0xc1, 0x04},
            {50, 0x01, 0x41, 0x02},
            {40, 0x02, 0xc1, 0x04},
            {30, 0x04, 0x81, 0x02},
            {15, 0x03, 0x41, 0x04},
        };
        static const struct rate_s rate_1[] = { /* 320x240 */
            {205, 0x01, 0xc1, 0x02}, /* 205 FPS: video is partly corrupt */
            {187, 0x01, 0x81, 0x02}, /* 187 FPS or below: video is valid */
            {150, 0x01, 0xc1, 0x04},
            {137, 0x02, 0xc1, 0x02},
            {125, 0x02, 0x81, 0x02},
            {100, 0x02, 0xc1, 0x04},
            {75, 0x03, 0xc1, 0x04},
            {60, 0x04, 0xc1, 0x04},
            {50, 0x02, 0x41, 0x04},
            {37, 0x03, 0x41, 0x04},
            {30, 0x04, 0x41, 0x04},
        };
        
        if (frame_width == 640) {
            r = rate_0;
            i = ARRAY_SIZE(rate_0);
        } else {
            r = rate_1;
            i = ARRAY_SIZE(rate_1);
        }
        while (--i > 0) {
            if (frame_rate >= r->fps)
                break;
            r++;
        }
        
        if (!dry_run) {
            sccb_reg_write(0x11, r->r11);
            sccb_reg_write(0x0d, r->r0d);
            ov534_reg_write(OV534_REG_CAMERA_CLK, r->re5);
        }
        
        debug("frame_rate: %d\n", r->fps);
        return r->fps;
    }
    
    void PS3EYECam::ov534_reg_write(uint16_t reg, uint8_t val)
    {
        //debug("reg=0x%04x, val=0%02x", reg, val);
        usb_buf[0] = val;

		uint8_t bmRequestType = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;

        int ret = libusb_control_transfer(handle_, // a handle for the device to communicate with
                                      bmRequestType, // the request type field for the setup packet
                                      0x01, // the request field for the setup packet
									  0x00, // the value field for the setup packet
									  reg, // the index field for the setup packet
                                      usb_buf, // a suitably-sized data buffer for either input or output (depending on direction bits within bmRequestType)
									  1, // the length field for the setup packet. The data buffer should be at least this size.
									  CTRL_TIMEOUT // timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0.
									  );

        if (ret < 0) {
            debug("write failed\n");
        }
    }
    
    uint8_t PS3EYECam::ov534_reg_read(uint16_t reg)
    {
		uint8_t bmRequestType = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;

        int ret = libusb_control_transfer(handle_,
										  bmRequestType,
										  0x01,
										  0x00,
										  reg,
										  usb_buf,
										  1,
										  CTRL_TIMEOUT
										  );
        

		//debug("reg=0x%04x, data=0x%02x", reg, usb_buf[0]);
        if (ret < 0) {
            debug("read failed\n");
            
        }

        return usb_buf[0];
    }
    
    int PS3EYECam::sccb_check_status()
    {
        for (int i = 0; i < 5; i++) {
            uint8_t data = ov534_reg_read(OV534_REG_STATUS);
            
            switch (data) {
                case 0x00:
                    return 1;
                case 0x04:
                    return 0;
                case 0x03:
                    break;
                default:
                    debug("sccb status 0x%02x, attempt %d/5\n", data, i + 1);
            }
        }
        return 0;
    }
    
    void PS3EYECam::sccb_reg_write(uint8_t reg, uint8_t val)
    {
        //debug("reg: 0x%02x, val: 0x%02x", reg, val);
        ov534_reg_write(OV534_REG_SUBADDR, reg);
        ov534_reg_write(OV534_REG_WRITE, val);
        ov534_reg_write(OV534_REG_OPERATION, OV534_OP_WRITE_3);
        
        if (!sccb_check_status())
            debug("sccb_reg_write failed\n");
    }
    
    
    uint8_t PS3EYECam::sccb_reg_read(uint16_t reg)
    {
        ov534_reg_write(OV534_REG_SUBADDR, reg);
        ov534_reg_write(OV534_REG_OPERATION, OV534_OP_WRITE_2);

        if (!sccb_check_status())
            debug("sccb_reg_read failed 1\n");
        
        ov534_reg_write(OV534_REG_OPERATION, OV534_OP_READ_2);
        if (!sccb_check_status())
            debug( "sccb_reg_read failed 2\n");
        
        return ov534_reg_read(OV534_REG_READ);
    }
	
    /* output a bridge sequence (reg - val) */
    void PS3EYECam::reg_w_array(const uint8_t (*data)[2], int len)
    {
        while (--len >= 0) {
            ov534_reg_write((*data)[0], (*data)[1]);
            data++;
        }
    }
    
    /* output a sensor sequence (reg - val) */
    void PS3EYECam::sccb_w_array(const uint8_t (*data)[2], int len)
    {
        while (--len >= 0) {
            if ((*data)[0] != 0xFF) {
                sccb_reg_write((*data)[0], (*data)[1]);
            } else {
                sccb_reg_read((*data)[1]);
                sccb_reg_write(0xFF, 0x00);
            }
            data++;
        }
    }
    
} // namespace
