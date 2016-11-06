
#ifndef XAPI_cpp
#define XAPI_cpp
#include <Xapi.h>
#include <util.h>


//************************************************************************
//************************************************************************
// Extracts the TUN packet
// Arguments:
//	buff: storage to return the buffer within
//  buff_sz: the max size of buff so not to overrun the buff
// Returns:
// 	The size of the returned buffer
uint8_t Xapi::get_TUN_packet(uint8_t* buff, const uint8_t buff_sz)
{
	uint8_t sz = 0;

	if(TUN_packet_exists())
		sz = m_external_TUN_single_buff.pop_top(buff, buff_sz);
	
	return sz;
}

//************************************************************************
//************************************************************************
// Returns TRUE if a TUN packet exists
// FALSE if a TUN packet doesn't exist
boolean Xapi::TUN_packet_exists()
{
	return m_external_TUN_single_buff.any_items();
}

//************************************************************************
//************************************************************************
// returns the TUN stored type
int Xapi::get_TUN_type()
{
	return (m_external_TUN_single_buff.get_TUN_type());
}

//************************************************************************
//************************************************************************
// determines if packet is ready
bool Xapi::is_xbee_TMP_packet_ready()
{	
	bool result = false;
	uint16_t packet_sz;
	
	// figure out if the reported size of the packet
	// matches the size we have read in
	if(m_TMP_bytes_read > SLOC_API_FRAME_TYPE_LOC)
	{
		// get the reported packet size
		packet_sz = m_util.get_SYS_packet_length(m_TMP_packet);
		
		//do the correction
		packet_sz += SYS_PACKET_SIZE_CORRECTION;
		
		// see if the packet sizes match
		if((packet_sz == m_TMP_bytes_read) && (packet_sz != 0))
			result = true;
	}
	
	return result;
}

//************************************************************************
//************************************************************************
// resets the packet state
void Xapi::TMP_reset_packet_state()
{
	m_TMP_bytes_read = 0;
	
	m_util.clean_packet(m_TMP_packet, LARGE_BUFF_SZ);
}

//************************************************************************
//************************************************************************
// Latch to capture XBee serial bytes and store them into
// the buffer
void Xapi::xapi_latch()
{
	uint8_t byte;

	// update the cooldown used for broadcasts
	update_cooldown();
	
	// see if there is a new byte
	if (m_xbee_serial.available() > 0) 
	{
		byte = m_xbee_serial.read();
		if((byte != SENT_XAPI_START_BYTE) && 
			(m_TMP_bytes_read < LARGE_BUFF_SZ-1) )
		{
			m_TMP_packet[m_TMP_bytes_read++] = byte;
			TUN_filter_packet(byte);
			
			//if we have a full packet, store it
			if(is_xbee_TMP_packet_ready())
			{
				m_SYS_single_buff.add_SYS_buffer(	m_TMP_packet,
													m_TMP_bytes_read);
											
				TMP_reset_packet_state();					
			}
		}
		else
		{
			TMP_reset_packet_state();
			m_TMP_packet[m_TMP_bytes_read++] = byte;
			TUN_filter_packet(byte);
		}
	}
}

//************************************************
//************************************************
// will reset the packet state
void Xapi::TUN_reset_external_packet_state()
{
	m_external_TUN_buff_index = 0;
	m_external_TUN_start_found = false;
	m_external_TUN_end_found = false;
	m_util.clean_packet(m_external_TUN_packet, LARGE_BUFF_SZ);
}

//************************************************
//************************************************
// filter out a PC protocol packet
// $[PAYLOAD TYPE][PAYLOAD SIZE][PAYLOAD]%
uint8_t Xapi::TUN_filter_packet(uint8_t c)
{
	uint8_t tx_byte = c;

	// first see if a byte is waiting

		// only add to buffer is byte is read
		if(true)
		{
			// see if byte is start byte
			if(tx_byte == SENT_START_BYTE)
			{
				// record that it's the start of a new packet
				TUN_reset_external_packet_state();
				m_external_TUN_start_found = true;
			}
			else if( tx_byte == SENT_END_BYTE)
			{
				// ensure we have read start byte
				// and there is more than 0 characters in buffer
				if( (m_external_TUN_start_found == true) && (m_external_TUN_buff_index > 0))
				{
					// we have a complete packet
					m_external_TUN_end_found = true;
					
					// store external packet if checksum is correct
					//if(m_util.verify_checksum(m_TUN_packet) )
						m_external_TUN_single_buff.add_TUN_buffer(m_external_TUN_packet, m_external_TUN_buff_index);
				}
			}
			else if( 	(m_external_TUN_start_found == true) &&
						(m_external_TUN_end_found == false) &&
						(m_external_TUN_buff_index < (LARGE_BUFF_SZ - 1)) )
						{
							// we have a single character that is
							// not a stop or a start
							// So just store it
							m_external_TUN_packet[m_external_TUN_buff_index++] = tx_byte;
						}
		}
	// return the byte we read
	return tx_byte;
}

//************************************************************************
//************************************************************************
// update the cooldown
void Xapi::update_cooldown()
{
	if(m_cooldown > 0)
		m_cooldown--;
	else
		m_cooldown = 0;
}

//************************************************************************
//************************************************************************
// Constructor
// Pass in the serial object that is connected to the XBee module.
Xapi::Xapi(HardwareSerial& _xbee_serial):
m_xbee_serial(_xbee_serial)
{
	TUN_reset_external_packet_state();
	TMP_reset_packet_state();

	m_SYS_single_buff = Single_buff();
	m_external_TUN_single_buff = Single_buff();
	m_local_TUN_single_buff = Single_buff();
	
	m_cooldown = 0;
}

//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
// The functions below are used to transmit packets. 
// The main workhorse of packet transmissions are of
// type 0x10 packets. These are TRANSMIT REQUESTS. These packets
// request the XBee to transmit a packet which carries
// a user-defined payload.
// In general, the user-defined payload will be of
// the packet protocol type. 

//************************************************************************
//************************************************************************
// Calculates checksum
// This routines assumes the CHECKSUM byte is set to API_SENTINAL_BYTE
// Using the sentinal tells the checksum when to complete the summation
uint8_t Xapi::calc_checksum(const uint8_t* packet)
{
	// summation
	uint16_t summ = 0;
	
	// the returned value
	uint8_t result = API_NULL_BYTE;
	
	// current byte
	uint8_t byte = API_NULL_BYTE;
	
	// index of where to start calculating
	uint16_t index = API_FRAME_TYPE_LOC;
	
	// loop while byte DNE API_SENTINAL_BYTE
	while (byte != SENT_XAPI_START_BYTE)
	{
		byte = packet[index++];
		if(byte != SENT_XAPI_START_BYTE)
		{
			summ += (uint16_t)byte;	
		}
	}
		
	// conclude the calculation
	result = (0xff - (summ & 0xff));
	
	return result;
}

//************************************************************************
//************************************************************************
// Sends a packet to the XBee
void Xapi::snd_packet(const uint8_t* packet, uint16_t size)
{
	m_xbee_serial.write( packet, size);
	
	m_xbee_serial.flush();
	
	// Don't slam the XBee
	delay(MISC_ANTI_SLAM);
}

//************************************************************************
//************************************************************************
// Figures out the length of the TRANSMISSION REQUEST PACKET (0x10)
// returns the string conversion of the 16bit length
// The payload must be passed in since that's the only variable
void Xapi::_0x10_get_length(const uint8_t* payload, uint8_t* length_str)
{
	// for the length
	uint16_t length = 0;
		
	length += strlen((const char*)payload);
	length += CORR_0x10_LENGTH_CORRECTION;
	
	m_util.int_to_hex(length,MISC_16BIT_HEX_SZ, (char*)length_str);
}

//************************************************************************
//************************************************************************
// This routine will construct a 0x10 type of packet which can then
// be sent to the XBee. The reason why it returns a buffer rather than just
// directly sending it to xbee is to debug the packet.
// Returns: The actual size of the 0x10 packet within the buffer "buff".
// Incoming:
// 
uint8_t Xapi::construct_transmit_req(const uint32_t addrMSB, 
									const uint32_t addrLSB, 
									const uint16_t addr16,
									const uint8_t* payload, 
									const uint8_t payload_sz,
									uint8_t* buff, 
									const uint8_t buff_sz)
{
	// outgoing packet
	uint8_t packet[LARGE_BUFF_SZ];
	
	// make sure packet is CLEAN
	m_util.clean_packet(packet, LARGE_BUFF_SZ);
	m_util.clean_packet(buff, buff_sz);
	
	// storage for addresses
	uint8_t addrMSB_str[SMALL_BUFF_SZ];
	uint8_t addrLSB_str[SMALL_BUFF_SZ];
	uint8_t addr16_str[SMALL_BUFF_SZ];
	
	// storage for length string
	uint8_t length_str[SMALL_BUFF_SZ];
	
	// get the length string (16-bit)
	_0x10_get_length(payload, length_str);
	
	// perform conversions of integers into hex ascii strings
	m_util.int_to_hex32(addrMSB, MISC_32BIT_HEX_SZ, (char*)addrMSB_str);
	m_util.int_to_hex32(addrLSB, MISC_32BIT_HEX_SZ, (char*)addrLSB_str);
	m_util.int_to_hex(addr16, MISC_16BIT_HEX_SZ, (char*)addr16_str);
	
	// begin packet construction
	packet[API_START_LOC] = SENT_XAPI_START_BYTE;
	packet[API_LENGTH_MSB_LOC] = m_util.ascii_hex_byte_to_int(length_str[BYTE0_MSB], length_str[BYTE0_LSB]);
	packet[API_LENGTH_LSB_LOC] = m_util.ascii_hex_byte_to_int(length_str[BYTE1_MSB], length_str[BYTE1_LSB]);
	packet[API_FRAME_TYPE_LOC] = API_FRAME_TX_REQUEST;
	packet[API_FRAME_ID_LOC] = 0x01;
	uint8_t loc = 5;
	
	// copy over the 64-bit address
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrMSB_str[BYTE0_MSB], addrMSB_str[BYTE0_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrMSB_str[BYTE1_MSB], addrMSB_str[BYTE1_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrMSB_str[BYTE2_MSB], addrMSB_str[BYTE2_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrMSB_str[BYTE3_MSB], addrMSB_str[BYTE3_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrLSB_str[BYTE0_MSB], addrLSB_str[BYTE0_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrLSB_str[BYTE1_MSB], addrLSB_str[BYTE1_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrLSB_str[BYTE2_MSB], addrLSB_str[BYTE2_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addrLSB_str[BYTE3_MSB], addrLSB_str[BYTE3_LSB]);
	
	// copy over the 16-bit address
	packet[loc++] = m_util.ascii_hex_byte_to_int(addr16_str[BYTE0_MSB], addr16_str[BYTE0_LSB]);
	packet[loc++] = m_util.ascii_hex_byte_to_int(addr16_str[BYTE1_MSB], addr16_str[BYTE1_LSB]);
	
	// set the broadcast radius
	packet[loc++] = API_BROADCAST_RADIUS_MAX;
	
	// set the options
	packet[loc++] = API_DEFAULT_OPTIONS;
	//packet[loc++] = 0x01;
	
	// ensure the constructed packet will not
	// overflow the allocated buffer
	if( (payload_sz + loc) > (LARGE_BUFF_SZ-1))
		return 0;
		
	if( (payload_sz + loc) > (buff_sz-1))
		return 0;
	
	// copy the payload over
	for(uint8_t i = 0; i < payload_sz; i++)
		packet[loc++] = payload[i];
	
	// set the checksum
	packet[loc] = SENT_XAPI_START_BYTE;
	packet[loc++] = calc_checksum(packet);		
	
	// copy over the packet
	m_util.cpy_buff(buff, packet, loc);
	
	// return the size of the buffer
	return loc;	
}

//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
// The functions below are used to connect to the Xapi_comms class. The purpose
// of Xapi_comms is to allow for the easy construction of services. The programmer
// needs to add Xapi_comms to the service in order to connect to Xapi.

//************************************************************************
//************************************************************************
// Allows for the distribution of a local TUN packet. Generally services will
// communicate with each other using the LOCAL TUN packet storage
uint8_t Xapi::CONNECT_local_TUN_set_packet(uint8_t* buff, const uint8_t buff_sz)
{
	uint8_t sz = 0;
	sz  = m_local_TUN_single_buff.add_TUN_buffer(buff, buff_sz);
	return sz;
}

//************************************************************************
//************************************************************************
// Gets the currently stored external TUN packet.
// Returns: > 0 or 0 depending on size of packet.
// If 0, that means there was no packet
uint8_t Xapi::CONNECT_local_TUN_get_packet(uint8_t* buff, const uint8_t buff_sz)
{
	uint8_t sz = 0;

	if(m_local_TUN_single_buff.any_items())
		sz = m_local_TUN_single_buff.pop_top(buff, buff_sz);
	
	return sz;
}

//************************************************************************
//************************************************************************
// gets the ID of the external tun packet
uint16_t Xapi::CONNECT_local_TUN_get_id()
{
	return (m_local_TUN_single_buff.get_buffer_id());
}

//************************************************************************
//************************************************************************
// gets the type of the external tun packet
// Returns type or -1 if no packet
int Xapi::CONNECT_local_TUN_get_type()
{
	return ( m_local_TUN_single_buff.get_TUN_type() );
}

//************************************************************************
//************************************************************************
// gets the type of the external tun packet
// Returns type or -1 if no packet
int Xapi::CONNECT_external_TUN_get_type()
{
	return (get_TUN_type());
}

//************************************************************************
//************************************************************************
// gets the ID of the external tun packet
uint16_t Xapi::CONNECT_external_TUN_get_id()
{
	return (m_external_TUN_single_buff.get_buffer_id());
}

//************************************************************************
//************************************************************************
// Gets the currently stored external TUN packet.
// Returns: > 0 or 0 depending on size of packet.
// If 0, that means there was no packet
uint8_t Xapi::CONNECT_external_TUN_get_packet(uint8_t* buff, const uint8_t buff_sz)
{
	uint8_t size = 0;
	
	size = get_TUN_packet(buff, buff_sz);
	
	return size;
}


#endif 