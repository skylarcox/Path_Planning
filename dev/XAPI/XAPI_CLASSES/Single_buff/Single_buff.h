
#ifndef SINGLE_BUFF_h
#define SINGLE_BUFF_h
#include <arduino.h>
#include <universal.h>
#include <util.h>

//**********************************************
//**********************************************
// state kept for single_buff

class Single_buff
{
	private:
		void reset_buffer();

	// maintains state of the buffer
	private:
		uint8_t m_buff_array[LARGE_BUFF_SZ];
		boolean m_buff_ready;
		uint8_t m_buff_sz;
		Util m_util;
		uint16_t m_buffer_id;
	
	// general routines to add and remove buffer
	public:
		Single_buff();
		uint8_t add_SYS_buffer(uint8_t* buff, const uint8_t sz);
		uint8_t add_TUN_buffer(uint8_t* buff, const uint8_t sz);	
		uint8_t add_TUN_buffer(uint8_t* buff, const uint8_t sz, const uint8_t ID);
		uint8_t pop_top(uint8_t* buff, const uint8_t sz);
		uint16_t get_buffer_id();
		boolean any_items();
		
	// routines for use on TUN packets.
	public:
		int get_TUN_type();
	 
};

#endif