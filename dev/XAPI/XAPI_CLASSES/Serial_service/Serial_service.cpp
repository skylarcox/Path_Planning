
#ifndef SERIAL_SERVICE_cpp
#define SERIAL_SERVICE_cpp

#include <serial_service.h>

//************************************************************************
//************************************************************************
// Simply sends a buffer over serial.
// Will add a frame to the buffer
void Serial_service::snd_serial_add_frame(const uint8_t* buff, uint8_t buff_sz)
{
	m_serial.print(SENT_START_BYTE);

	m_serial.write(buff, buff_sz);
	
	m_serial.print(SENT_END_BYTE);
	
	m_serial.flush();
}

//************************************************************************
//************************************************************************
// Process any local serial messages
void Serial_service::snd_local_TUN_packet_via_serial()
{
	uint8_t packet_sz = 0;
	
	// storage for the debug packet
	uint8_t serial_packet[LARGE_BUFF_SZ];
		
	// clean packet
	m_util.clean_packet(serial_packet, LARGE_BUFF_SZ);
		
	// extract the packet
	m_xapi.CONNECT_local_TUN_get_packet(serial_packet, LARGE_BUFF_SZ);
		
	// get the size of the packet
	packet_sz = m_util.get_TUN_packet_sz(serial_packet);
		
	// ship out the packet through serial
	snd_serial_add_frame(serial_packet, packet_sz);
}

//************************************************************************
//************************************************************************
// Latch to sample the serial hardware
void Serial_service::serial_service_latch()
{
	uint8_t packet_type = 0;

	// see if there is a new byte
	if (m_serial.available() > 0) 
	{
		assemble_TUN_packet(m_serial.read());
	}
	
	// process any local serial packets that need to
	// be shipped out over serial. 
	// NOTE: the point of this code is to allow other
	// services to ship out packets via serial instead
	// of radio.
	packet_type = m_xapi.CONNECT_local_TUN_get_type();
	switch(packet_type)
	{
		case TUN_TYPE_LOCAL_SERIAL_DEBUG_MSG:
		case TUN_TYPE_LOCAL_CHAT:
			snd_local_TUN_packet_via_serial();
		break;
	}
}

//************************************************************************
//************************************************************************
// This routine allows for the serial service to display
// a simple local LCD message.
// The payload is used to create an entirely new LCD packet.
// The new derived packet is of type TUN_TYPE_LOCAL_LCD_MSG
// packet format: 
// [TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]
void Serial_service::simple_local_LCD_msg(	uint8_t* buff, 
											uint8_t buff_sz)
{
	// storage for payload
	uint8_t payload[MED_BUFF_SZ];
	
	// storage for new packet
	uint8_t new_TUN_packet[LARGE_BUFF_SZ];
	uint8_t new_TUN_packet_sz = 0;
	
	// extract payload
	m_util.get_TUN_payload(buff, payload, MED_BUFF_SZ);
	
	// create the TUN packet
	new_TUN_packet_sz = m_util.create_TUN_lcd_packet(	true, 0, 0, payload,
														strlen((char*)payload),
														new_TUN_packet,
														LARGE_BUFF_SZ);

	// send the new TUN packet out locally
	m_xapi.CONNECT_local_TUN_set_packet(new_TUN_packet, new_TUN_packet_sz);
}

//************************************************************************
//************************************************************************
// This routine processes a completely assembled RX TUN packet.
// Returns:
// true: packet was processed
// false: packet was not processed due to CHECKSUM error
boolean Serial_service::process_TUN_packet(	uint8_t* buff, 
											uint8_t buff_sz)
{
	boolean success = false;
	uint8_t TUN_type = ILLEGAL_TUN_TYPE;
	
	// only process the buffer if passes CHECKSUM
	if(m_util.verify_checksum(buff))
	{
		TUN_type = m_util.get_TUN_type(buff);
		
		switch(TUN_type)
		{
			// an incoming request to use the local
			// LCD screen to display a message.
			case TUN_TYPE_LOCAL_LCD_MSG:
				m_xapi.CONNECT_local_TUN_set_packet(buff, buff_sz);
			break;	
			
			// for doing a simple local LCD debug message
			case TUN_TYPE_LOCAL_SIMPLE_LCD_MSG:
				simple_local_LCD_msg(buff, buff_sz);
			break;	
		}
	
		success = true;
	}
	return success;
}

//************************************************************************
//************************************************************************
// This routine resets the entire RX state and
// makes the service ready to RX a new TUN packet.
void Serial_service::reset_rx_state()
{
	m_util.clean_packet(m_rx_buff, LARGE_BUFF_SZ);
	m_rx_start_found = false;
	m_rx_end_found = false;
	m_rx_buff_ready = false;
	m_rx_buff_index = 0;
}

//************************************************************************
//************************************************************************
// This routine takes in RX bytes and assembles a TUN packet.
// Format of the TUNNELED (TUN) packet in ASCII-HEX:
// $[TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]%
uint8_t Serial_service::assemble_TUN_packet(uint8_t c)
{
	uint8_t rx_byte = c;

	// first see if a byte is waiting

		// only add to buffer is byte is read
		if(c != '\0')
		{
			// see if byte is start byte
			if(rx_byte == SENT_START_BYTE)
			{
				// record that it's the start of a new packet
				reset_rx_state();
				m_rx_start_found = true;				
			}
			else if( rx_byte == SENT_END_BYTE)
			{	
				// ensure we have read start byte
				// and there is more than 0 characters in buffer
				if( (m_rx_start_found == true) && (m_rx_buff_index > 0))
				{
					// we have a complete packet
					m_rx_end_found = true;
					m_rx_buff_ready = true;
					
					// process the packet
					process_TUN_packet(m_rx_buff, m_rx_buff_index);
					
					// reset the buffer
					reset_rx_state();
				}
			}
			else if( 	(m_rx_start_found == true) &&
						(m_rx_end_found == false) &&
						(m_rx_buff_index < (LARGE_BUFF_SZ - 1)) )
						{
							// we have a single character that is
							// not a stop or a start
							// So just store it
							m_rx_buff[m_rx_buff_index++] = rx_byte;
						}
		}
		
	// return the byte we read
	return rx_byte;
}

//************************************************
//************************************************
Serial_service::Serial_service(HardwareSerial& _serial, Xapi& _xapi):
m_serial(_serial), m_xapi(_xapi)
{
	m_util = Util();
	reset_rx_state();
}

#endif