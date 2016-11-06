
#ifndef LCD_SERVICE_cpp
#define LCD_SERVICE_cpp
#include <LCD_service.h>

//***************************************************
//***************************************************
void LCD_service::lcd_service_latch()
{
	// update the button states
	read_LCD_buttons();
	
	// process any local LCD message packets
	process_local_TUN_packet();
	
	// process any external LCD message packets
	process_external_TUN_packet();
}

//***************************************************
//***************************************************
// This routine will send a local DEBUG message
// to the Serial_service
uint8_t LCD_service::lcd_snd_local_serial_debug(const uint8_t* msg)
{
	// message size
	uint8_t msg_sz = strlen((char*)msg);
	
	// packet size
	uint8_t packet_sz = 0;
	
	// storage for packet
	uint8_t packet[LARGE_BUFF_SZ];
	
	// clean the buffer
	m_util.clean_packet(packet, LARGE_BUFF_SZ);
	
	// construct the packet
	packet_sz = m_util.create_TUN_serial_packet(	TUN_TYPE_LOCAL_SERIAL_DEBUG_MSG,
													msg, 
													msg_sz,
													packet,
													LARGE_BUFF_SZ);

	// ship the packet off locally
	m_xapi.CONNECT_local_TUN_set_packet(packet, packet_sz);
													
	return packet_sz;
}


//***************************************************
//***************************************************
// This routine will query the XAPI to see
// if there is a local message waiting for
// the LCD. If so, we need to grab it and display it.
void LCD_service::process_external_TUN_packet()
{
	// see if there is a packet waiting
	if(m_xapi.CONNECT_external_TUN_get_type() == TUN_TYPE_EXTERNAL_LCD_MSG)
	{	
		// allocate the space
		uint8_t TUN_packet[MED_BUFF_SZ];
	
		// extract the packet
		m_xapi.CONNECT_external_TUN_get_packet(TUN_packet, MED_BUFF_SZ);
	
		// display the contents of the TUN packet
		display_TUN_packet(TUN_packet);
	}
}

//***************************************************
//***************************************************
// This routine will query the XAPI to see
// if there is a local message waiting for
// the LCD. If so, we need to grab it and display it.
void LCD_service::process_local_TUN_packet()
{
	// see if there is a packet waiting
	if(m_xapi.CONNECT_local_TUN_get_type() == TUN_TYPE_LOCAL_LCD_MSG)
	{	
		// allocate the space
		uint8_t TUN_packet[MED_BUFF_SZ];
	
		// extract the packet
		m_xapi.CONNECT_local_TUN_get_packet(TUN_packet, MED_BUFF_SZ);
	
		// display the contents of the TUN packet
		display_TUN_packet(TUN_packet);
	}
}

//***************************************************
//***************************************************
// This routine processes the stored TUN packet
// All relevant LCD data is extracted from the
// PAYLOAD section of the TUN packet.
// returns:
//   The size of the string being displayed

uint8_t LCD_service::display_TUN_packet(uint8_t* m_TUN_storage)
{
	// x and y coordinates
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t LCD_payload_sz = 0;

	// temp storage
	uint8_t LCD_payload[MED_BUFF_SZ];
	
	// extract the LCD packet
	LCD_payload_sz = m_util.get_TUN_payload(m_TUN_storage, LCD_payload, MED_BUFF_SZ);
	
	if(LCD_payload_sz != 0)
	{
	
		// get the X and Y coordinates
		x = m_util.hex_to_int(LCD_X_START, LCD_X_END, LCD_X_SZ, LCD_payload);
		y = m_util.hex_to_int(LCD_Y_START, LCD_Y_END, LCD_Y_SZ, LCD_payload);
	
		// figure out the msg size
		LCD_payload_sz -= (LCD_X_SZ + LCD_Y_SZ);
	
		// clear the line
		clear_line(x);
	
		// set the cursor
		lcd_setCursor(x, y);
	
		// display the string
		for(uint8_t i = 0; i < LCD_payload_sz; i++)
			lcd_putc((const char)LCD_payload[i+LCD_MSG_START]);
	}
}

//***************************************************
//***************************************************
// This routine clears one complete line
void LCD_service::clear_line(uint8_t x)
{
	lcd_setCursor(x, 0);
	
	char white_space = ' ';
	
	// display the string
	for(uint8_t i = 0; i < MISC_LCD_WIDTH; i++)
		lcd_putc(white_space);
}

//***************************************************
//***************************************************
// This routine simplifies the process of sending
// a LOCAL LCD message. While it seems odd to send
// a local LCD message from within the LCD service, 
// it will serve as a template for other services.
//
// Returns the size of the entire LCD TUN packet.
// NOTE: The packet will be transferred to the XAPI
uint8_t LCD_service::lcd_snd_LOCAL_message(	const uint8_t x, 
											const uint8_t y, 
											const uint8_t* str)
{
	uint8_t packet_sz = 0;

	// storage for the LCD payload and packet
	uint8_t packet[LARGE_BUFF_SZ];
	
	// clean the buffers
	m_util.clean_packet(packet, LARGE_BUFF_SZ);
	
	// derive the packet
	packet_sz = m_util.create_TUN_lcd_packet(	true, x, y, 
												str, 
												strlen((char*)str),
												packet,
												LARGE_BUFF_SZ);
			
	// store the packet in the local XAPI message buffer
	m_xapi.CONNECT_local_TUN_set_packet(packet, packet_sz);
		
	return packet_sz;
}

//***************************************************
//***************************************************
// This routine simplifies the process of sending
// a message from one LCD to another via the
// XBee modules. Consider using this routine
// instead of trying to send a message using
// all of the individual steps.
// Incoming:
//	addrLSB: The lower-half 32-bit of the address.
//	x: Column of the LCD panel (values 0-15) 
//	y: Row of the LCD panel (values 0-1)
//	msg: the actual message in standard
//         c-type string with null termination.
// Returns:
//    The size of the complete packet to be sent
uint8_t LCD_service::lcd_snd_EXTERNAL_message(	const uint32_t addrMSB,
												const uint32_t addrLSB,
												const uint16_t addr16,
												const uint8_t x,
												const uint8_t y,
												const uint8_t* msg)
{
	uint8_t payload_buff_sz = 0;
	uint8_t TUN_buff_sz = 0;
	uint8_t Xbee_buff_sz = 0;
	
	uint8_t payload_buff[LARGE_BUFF_SZ];
	uint8_t TUN_buff[LARGE_BUFF_SZ];
	uint8_t Xbee_buff[LARGE_BUFF_SZ];
	
	// produce the following:
	// [X:2][Y:2][STRING] = [PAYLOAD]
	payload_buff_sz = m_util.construct_lcd_payload(	x, 
													y, 
													msg, 
													strlen((const char*)msg),
													payload_buff,
													LARGE_BUFF_SZ);

	// produce the following (a TUN packet):
	// $[TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD]%
	TUN_buff_sz = m_util.create_TUN_packet(	TUN_TYPE_EXTERNAL_LCD_MSG, 
											payload_buff, 
											payload_buff_sz, 
											TUN_buff, 
											LARGE_BUFF_SZ);
	
	// produce the following (a complete Xbee packet)
	// [PREAMBLE][%TUN_PACKET%][CHECKSUM]
	Xbee_buff_sz = m_xapi.construct_transmit_req(	addrMSB, 
													addrLSB, 
													addr16,
													TUN_buff, 
													TUN_buff_sz,
													Xbee_buff, 
													LARGE_BUFF_SZ);

	// Physically ship out completed Xbee packet over radio
	m_xapi.snd_packet(Xbee_buff, Xbee_buff_sz);
	
	return Xbee_buff_sz;
}
		
//***************************************************
//***************************************************
void LCD_service::lcd_print(const int x, const int y, 
								const char* str_msg)
{
	lcd_setCursor(x, y);
	lcd_print(str_msg);
}

//***************************************************
//***************************************************
void LCD_service::lcd_putc(const char c)
{
	m_lcd->write(c);
}

//***************************************************
//***************************************************
void LCD_service::lcd_print(const char* str_msg)
{
	m_lcd->print(str_msg);
}

//***************************************************
//***************************************************
void LCD_service::lcd_setCursor(const int x,const int y)
{
	m_lcd->setCursor(x,y);
}

//***************************************************
//***************************************************
int LCD_service::get_lcd_key()
{
	return m_lcd_key;
}

//***************************************************
//***************************************************
// Resets the TUN packet storage
void LCD_service::reset_TUN_storage()
{
	// obsolete
}

//***************************************************
//***************************************************
LCD_service::LCD_service(Xapi& _xapi):
m_xapi(_xapi)
{
	m_lcd = new LiquidCrystal(8, 9, 4, 5, 6, 7);
	m_lcd_key = LCD_btnNONE;	
	m_locked = false;
	reset_TUN_storage();
	m_prev_lcd_key = LCD_btnNONE;
	m_lcd->begin(16, 2);             
	lcd_setCursor(0,0);
	lcd_print("HOW DO YOU FEEL?"); 
}

//***************************************************
//***************************************************
int LCD_service::read_LCD_buttons()
{
	int adc_key_in;

	adc_key_in = analogRead(0);   

	if (adc_key_in > 1000)
	{	
		m_lcd_key = LCD_btnNONE; 
		m_locked = false;
		return m_lcd_key;
	}

	if(m_locked)
	{
		m_lcd_key = LCD_btnNONE;
		return LCD_btnNONE;
	}
	
	if (adc_key_in < 50) 
	{
		m_locked = true;
		m_lcd_key = LCD_btnRIGHT; 
		return m_lcd_key;
	}
		
	if (adc_key_in < 250)
	{	
		m_locked = true;
		m_lcd_key = LCD_btnUP;
		return m_lcd_key;
	}
		
	if (adc_key_in < 450)
	{	
		m_locked = true;
		m_lcd_key = LCD_btnDOWN; 	
		return m_lcd_key;
	}
		
	if (adc_key_in < 650)
	{	
		m_locked = true;
		m_lcd_key = LCD_btnLEFT;  
		return m_lcd_key;
	}		
	
	if (adc_key_in < 850)
	{
		m_locked = true;
		m_lcd_key = LCD_btnSELECT;
		return m_lcd_key;
	}
}

#endif