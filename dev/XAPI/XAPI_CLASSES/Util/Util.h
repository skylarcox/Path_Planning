#ifndef UTIL_h
#define UTIL_h
#include <arduino.h>


//***********************************************
//***********************************************
// UTIL. This file contains utility functions
// that can be utilized by all code within 
// the system. 

class Util
{
	private:
		
	
	// Generic routines useful to all
	public:
		void int_to_hex32(uint32_t value, uint32_t size, char stor_buff[]);
		void int_to_hex(const int value, const int size, char stor_buff[]);
		uint8_t ascii_hex_byte_to_int(const uint8_t msb, const uint8_t lsb);
		uint16_t hex_to_int(int start_byte, int end_byte, int size, const uint8_t* buffer);
		void clean_packet(uint8_t* packet, uint16_t size);
		uint8_t cpy_buff(uint8_t* dest, const uint8_t* src, const uint8_t byte_cnt);
		boolean verify_checksum(const uint8_t* packet);
		
	// Routines generally only useful on SYS packet types
	public:
		uint8_t get_SYS_packet_length(const uint8_t* packet);
		
	// routines specific to TUN packets
	public:
		uint8_t create_TUN_packet(uint8_t, const uint8_t*,uint8_t, uint8_t*, uint8_t);
		uint8_t get_TUN_type(const uint8_t* packet);
		uint8_t get_TUN_payload_sz(const uint8_t* packet);
		uint8_t get_TUN_packet_sz(const uint8_t* packet);
		boolean is_TUN_packet_framed(const uint8_t* packet);
		boolean remove_TUN_frame(uint8_t* packet);
		uint16_t get_TUN_checksum(const uint8_t* packet);
		uint16_t derive_TUN_checksum(const uint8_t* packet);
		uint8_t get_TUN_payload(const uint8_t* packet, uint8_t* buff, uint8_t buff_sz);
		
	// routine to create a LCD TUN packet (either local or external)
	public:
		uint8_t construct_lcd_payload(	const uint8_t x, 
										const uint8_t y, 
										const uint8_t* str, 
										const uint8_t str_sz,
										uint8_t* buff,
										const uint8_t buff_sz);
										
		uint8_t create_TUN_lcd_packet(	const boolean is_local,
										const uint8_t x, 
										const uint8_t y, 
										const uint8_t* str, 
										const uint8_t str_sz,
										uint8_t* buff,
										const uint8_t buff_sz);
										
	// routines to send a TUN packet through serial
	public:
		uint8_t create_TUN_serial_packet(	const uint8_t TUN_type,
											const uint8_t* str, 
											const uint8_t str_sz,
											uint8_t* buff,
											const uint8_t buff_sz);
	
	// Constructors
	public:
		Util();
};

#endif