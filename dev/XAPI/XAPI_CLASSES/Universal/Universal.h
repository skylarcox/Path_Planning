#ifndef UNIVERSAL_h
#define UNIVERSAL_h

//***********************************************
//***********************************************
// Format of the TUNNELED (TUN) packet in ASCII-HEX:
// $[TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]%

//***********************************************
//***********************************************
// API byte locations in buffers going to/from XBEE
#define API_START_LOC 0
#define API_LENGTH_MSB_LOC 1
#define API_LENGTH_LSB_LOC 2
#define API_FRAME_TYPE_LOC 3
#define API_FRAME_ID_LOC 4
#define API_BROADCAST_RADIUS_MAX 0x00
#define API_DEFAULT_OPTIONS 0x00
#define API_NULL_BYTE 0x00
#define API_FRAME_TX_REQUEST 0x10

//***********************************************
//***********************************************
// This file contains all the #defines needed
// by the Xapi and services. The intent is that
// everyone has the same values for buffer sizes,
// SYS packet types and TUN packet types. Therefore,
// there is no confusion or memory buffer mismatches.
// Please update this file to optimize memory usage.

//***********************************************
//***********************************************
// Standard Buffer sizes.
// These buffer sizes must be used by all services
// and the Xapi to ensure there is no buffer size
// mismatches. Please keep buffer sizes to three 
// buffer sizes max. The more there are defined,
// the more confusing the system becomes.
//
// WARNING: If you modify these values, you MUST
// modify those values on the PC as well.
#define LARGE_BUFF_SZ 70
#define MED_BUFF_SZ 40
#define SMALL_BUFF_SZ 20
#define _8_BIT_SZ 3
#define _16_BIT_SZ 5
#define _32_BIT_SZ 9

//***********************************************
//***********************************************
// System (SYS) packet types
#define SYS_PACKET_SIZE_CORRECTION 4

//***********************************************
//***********************************************
// Tunnelled (TUN) packet types
// Note: All packet types must be listed here
// This will help keep type allocations straight

// illegal TUN type
#define ILLEGAL_TUN_TYPE 0x00

// to ship out a LCD messages
#define TUN_TYPE_EXTERNAL_LCD_MSG 0x01
#define TUN_TYPE_LOCAL_LCD_MSG 0x02

// For simple local LCD display (debug)
#define TUN_TYPE_LOCAL_SIMPLE_LCD_MSG 0x03
#define TUN_TYPE_EXTERNAL_SIMPLE_LCD_MSG 0x04

// Reserved
#define TUN_TYPE_RESERVED 0x05

// for serial debug messages
#define TUN_TYPE_EXTERNAL_SERIAL_DEBUG_MSG 0x06
#define TUN_TYPE_LOCAL_SERIAL_DEBUG_MSG 0x07

// for chat
#define TUN_TYPE_EXTERNAL_CHAT 0x08
#define TUN_TYPE_LOCAL_CHAT 0x09


//***********************************************
//***********************************************
// XBee Byte Locations (XLOC) for outgoing packets
// to the XBee module via a serial connection

//***********************************************
//***********************************************
// System packet byte locations (SLOC) for incoming
// packets from the xbee module.
#define SLOC_API_FRAME_TYPE_LOC 3
#define SLOC_API_LENGTH_MSB_LOC 1
#define SLOC_API_LENGTH_LSB_LOC 2

//***********************************************
//***********************************************
// Tunnelled packet byte locations (TLOC) for incoming
// packets from the xbee module.

//***********************************************
//***********************************************
// Sentinel bytes (SENT) used throughout the system.
#define SENT_XAPI_START_BYTE 0x7E
#define SENT_START_BYTE '$'
#define SENT_END_BYTE '%'
#define SENT_API_NULL_BYTE 0x00
#define SENT_CHAR_NULL_BYTE '\0'
#define SENT_NULL_TUN_TYPE ILLEGAL_TUN_TYPE

//***********************************************
//***********************************************
// Return values (RET) used throughout the system.
#define RET_NEGATIVE -1
#define RET_ZERO 0

//***********************************************
//***********************************************
// Misc values (MISC) used throughout the system.
#define MISC_SERIAL_SPEED 57600
#define MISC_PC_SPEED 9600
#define MISC_8BIT_HEX_SZ 2
#define MISC_16BIT_HEX_SZ 4
#define MISC_32BIT_HEX_SZ 8
#define MISC_ANTI_SLAM 220
#define MISC_TUN_HEADER_SZ 8
#define MISC_TUN_HEADER_NO_CHECKSUM_SZ 4
#define MISC_TUN_START_STOP_SZ 2
#define MISC_LCD_WIDTH 16


//***********************************************
//***********************************************
// LCD values that are intended to be used
// with the LCD service (LCD)
// REMEBER: Any new packet types of this service
// should be filed under the TUN packet types
// section. All service packet types are of type
// TUN. They are TUNneled packets.

#define LCD_btnRIGHT  0
#define LCD_btnUP     1
#define LCD_btnDOWN   2
#define LCD_btnLEFT   3
#define LCD_btnSELECT 4
#define LCD_btnNONE   5
#define LCD_SCREEN_WIDTH 16
#define LCD_ROW_0 0
#define LCD_ROW_1 1
#define LCD_X_START 0
#define LCD_X_END 1
#define LCD_X_SZ 2
#define LCD_Y_START 2
#define LCD_Y_END 3
#define LCD_Y_SZ 2
#define LCD_MSG_START 4



//***********************************************
//***********************************************
// Locations into the TUN type necessary for 
// extractions
// Format of the TUNNELED (TUN) packet in ASCII-HEX:
// [TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]

#define TUN_TYPE_START 0
#define TUN_TYPE_END 1
#define TUN_TYPE_SZ 2

#define TUN_PAYLOAD_SZ_START 2
#define TUN_PAYLOAD_SZ_END 3
#define TUN_PAYLOAD_SZ_SZ 2

#define TUN_CHECKSUM_START 4
#define TUN_CHECKSUM_END 7
#define TUN_CHECKSUM_SZ 4

#define TUN_PAYLOAD_START 8


//***********************************************
//***********************************************
// Addresses used for broadcasts

#define MSB_BROADCAST_ADDR 0x00000000
#define LSB_BROADCAST_ADDR 0x0000FFFF
#define ADDR16_BROADCAST 0xFFFE
#define DH_BROADCAST 0x0
#define DL_BROADCAST 0xFFFF
#define ADDR_MSB 0x0013a200
#define BYTE0_MSB 0
#define BYTE0_LSB 1
#define BYTE1_MSB 2
#define BYTE1_LSB 3
#define BYTE2_MSB 4
#define BYTE2_LSB 5
#define BYTE3_MSB 6
#define BYTE3_LSB 7

//***********************************************
//***********************************************
// Length correction for different packets (CORR).
#define CORR_0x10_LENGTH_CORRECTION 14

#define DEBUG_MSB_ADDR 0x0013a200
#define DEBUG_LSB_ADDR 0x40a1446d
#define DEBUG_ADDR16 0xFFFE

//***********************************************
//***********************************************
// Used by Subscriptions (SUB)
#define SUB_MAX_SUBSCRIPTIONS 5


#endif