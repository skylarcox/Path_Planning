#ifndef XAPI_h
#define XAPI_h
#include <arduino.h>
#include <single_buff.h>
#include <util.h>

//***********************************************
//***********************************************
// XAPI - the lowest level driver.
// This API will simply TX/RX from the XBee module.
// For further functionality, services must be added.
// Only create one instance of this class and pass it
// by reference to the services.

class Xapi
{
	// private variables for TMP/SYS packets
	// EXTERNAL SYS PACKET
	private:
		uint8_t m_TMP_packet[LARGE_BUFF_SZ];
		uint16_t m_TMP_bytes_read;	
		Single_buff m_SYS_single_buff;

	// private variables dealing with TUN packets
	// EXTERNAL TUN PACKET
	private:
		uint8_t m_external_TUN_packet[LARGE_BUFF_SZ];
		boolean m_external_TUN_start_found;
		boolean m_external_TUN_end_found;
		uint8_t m_external_TUN_buff_index;
		Single_buff m_external_TUN_single_buff;
		
	// private variables dealing with local TUN packets
	// LOCAL TUN PACKET
	private:
		Single_buff m_local_TUN_single_buff;
		
	// Generic private variables
	private:
		HardwareSerial& m_xbee_serial;
		uint32_t m_cooldown;
		Util m_util;

	// Private functions dealing with SYS/TMP packets
	private:
		void TMP_reset_packet_state();
		bool is_xbee_TMP_packet_ready();
		uint8_t get_TMP_packet_length(const uint8_t* packet);
	
	// Private functions dealing with TUN packets
	private:
		uint8_t TUN_filter_packet(uint8_t c);
		void TUN_reset_external_packet_state();
		
	// public functions to send 0x10 Transmit Request packet
	public:
		void snd_packet(const uint8_t* packet, uint16_t size);
		void _0x10_get_length(const uint8_t* payload, uint8_t* length_str);
		uint8_t construct_transmit_req(	const uint32_t addrMSB, 
										const uint32_t addrLSB, 
										const uint16_t addr16,
										const uint8_t* payload, 
										const uint8_t payload_sz,
										uint8_t* buff, 
										const uint8_t buff_sz);

	// Generic private functions
	private:
		void update_cooldown();
		uint8_t calc_checksum(const uint8_t* packet);
	 
	 // public functions to connect to the Xapi_comms class
	 public:
	 
		// external TUN
		int CONNECT_external_TUN_get_type();
		uint16_t CONNECT_external_TUN_get_id();
		uint8_t CONNECT_external_TUN_get_packet(uint8_t* buff, const uint8_t buff_sz);
		
		// local TUN
		int CONNECT_local_TUN_get_type();
		uint16_t CONNECT_local_TUN_get_id();
		uint8_t CONNECT_local_TUN_get_packet(uint8_t* buff, const uint8_t buff_sz);
		uint8_t CONNECT_local_TUN_set_packet(uint8_t* buff, const uint8_t buff_sz);
	 
	// All public functions
	public:
		Xapi(HardwareSerial& _xbee_serial);
		void xapi_latch();
		boolean TUN_packet_exists();
		uint8_t get_TUN_packet(uint8_t* buff, const uint8_t buff_sz);
		int get_TUN_type();
};

#endif