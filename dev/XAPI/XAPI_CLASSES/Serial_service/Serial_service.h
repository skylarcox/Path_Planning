
#ifndef SERIAL_SERVICE_h
#define SERIAL_SERVICE_h

#include <arduino.h>
#include <single_buff.h>
#include <util.h>
#include <xapi.h>

//**********************************************
//**********************************************
// The Serial Service allows for the XAPI to
// communicate over a standard serial connection
// to the outside world. This service only
// communicates via the standard TUN packet.
// TUN packet format:
// Format of the TUNNELED (TUN) packet in ASCII-HEX:
// $[TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]%


class Serial_service
{
	// The objects this service requires
	private:
		Util m_util;
		HardwareSerial& m_serial;
		Xapi& m_xapi;
		
	// This buffer and variables keep track of 
	// incoming RX bytes.
	private:
		uint8_t m_rx_buff[LARGE_BUFF_SZ];
		boolean m_rx_start_found;
		boolean m_rx_end_found;
		boolean m_rx_buff_ready;
		uint8_t m_rx_buff_index;
	
	// routines to maintain state
	private:
		uint8_t assemble_TUN_packet(uint8_t c);
		void reset_rx_state();
		boolean process_TUN_packet(	uint8_t* buff, uint8_t buff_sz);
		void simple_local_LCD_msg( uint8_t* buff, uint8_t buff_sz);
		void snd_serial_add_frame(const uint8_t* buff, uint8_t buff_sz);
		void snd_local_TUN_packet_via_serial();
	
	// constructor and latch
	public:
		Serial_service(HardwareSerial& _serial, Xapi& _xapi);
		void serial_service_latch();
};

#endif