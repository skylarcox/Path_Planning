
#ifndef LCD_SERVICE_h
#define LCD_SERVICE_h
#include <arduino.h>
#include <LiquidCrystal.h>
#include <Xapi.h>
#include <Util.h>

//************************************************
//************************************************
// Steps to ship out a packet via XBee
// to a remote LCD panel
// 1) construct_lcd_payload
// 2) util.create_TUN_packet
// 3) xapi.construct_transmit_req
// 4) xapi.snd_packet

//**********************************************
//**********************************************

class LCD_service
{
	// objects used
	private:
		LiquidCrystal* m_lcd;
		Xapi& m_xapi;
		Util m_util;
		
	// button state variables
	private:
		boolean m_locked;
		int m_lcd_key;
		int m_prev_lcd_key;
		
	// functions for LCD
	private:
		void reset_TUN_storage();
		int read_LCD_buttons();
		uint8_t display_TUN_packet(uint8_t* m_TUN_storage);
		void process_local_TUN_packet();
		void process_external_TUN_packet();
		void clear_line(uint8_t x);
		
	// sending and receiving a LCD packet is complex.
	// These public routines will simplify the
	// process of sending and displaying
	// a message.
	public:
		uint8_t lcd_snd_EXTERNAL_message(	const uint32_t addrMSB,
											const uint32_t addrLSB,
											const uint16_t addr16,
											const uint8_t x,
											const uint8_t y,
											const uint8_t* msg);
		
		uint8_t lcd_snd_LOCAL_message(	const uint8_t x, 
										const uint8_t y, 
										const uint8_t* str);
		
		uint8_t lcd_snd_local_serial_debug(const uint8_t* msg);
		
	// general functions for lcd and buttons
	public:
		void lcd_print(const int x, const int y, const char* str_msg);
		void lcd_print(const char* str_msg);
		void lcd_putc(const char c);
		int get_lcd_key();
		boolean is_rx_TUN_ready();
		void lcd_setCursor(const int x, const int y);
		void lcd_service_latch();
		
	// Constructor
	public:
		LCD_service(Xapi& _xapi);
};

#endif