using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SerialPortTerminal
{
    //***********************************************
    //***********************************************
    // standard buffer sizes
    enum BUFF_sizes 
    { 
        LARGE_BUFF_SZ = 70, MED_BUFF_SZ = 40, SMALL_BUFF_SZ = 20
    }

    //***********************************************
    //***********************************************
    // misc values
    enum MISC_values
    {
        MISC_SERIAL_SPEED = 57600, MISC_8BIT_HEX_SZ = 2, MISC_16BIT_HEX_SZ = 4,
        MISC_32BIT_HEX_SZ = 8, MISC_ANTI_SLAM = 220, CHECKSUM_ERROR = -1
    }

    //***********************************************
    //***********************************************
    // Locations into the TUN type necessary for 
    // extractions
    // Format of the TUNNELED (TUN) packet in ASCII-HEX:
    // [TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]
    enum TUN_locations 
    {
        TUN_TYPE_START=0, TUN_TYPE_END = 1, TUN_TYPE_SZ = 2,
        TUN_PAYLOAD_SZ_START=2, TUN_PAYLOAD_SZ_END=3, TUN_PAYLOAD_SZ_SZ=2,
        TUN_CHECKSUM_START=4, TUN_CHECKSUM_END=7, TUN_CHECKSUM_SZ=4,
        TUN_PAYLOAD_START=8
    }
   
    //***********************************************
    //***********************************************
    // Addresses used for broadcasts
    enum ADDR_bc
    {
        MSB_BROADCAST_ADDR=0x00000000, LSB_BROADCAST_ADDR=0x0000FFFF,
        ADDR16_BROADCAST=0xFFFE
    }

    //***********************************************
    //***********************************************
    // Sentinel bytes (SENT) used throughout the system.
    enum SENT_values
    {
        SENT_START_BYTE='$', SENT_END_BYTE='%', SENT_API_NULL_BYTE=0x00,
        SENT_CHAR_NULL_BYTE='\0'
    }

    //***********************************************
    //***********************************************
    // TUN packet types
    enum TUN_types
    {
        ILLEGAL_TUN_TYPE = 0x00, TUN_TYPE_EXTERNAL_LCD_MSG = 0x01, 
        TUN_TYPE_LOCAL_LCD_MSG = 0x02, TUN_TYPE_LOCAL_SIMPLE_LCD_MSG = 0x03,
        TUN_TYPE_CHAT = 0x05
    }
 

}
