
#ifndef UTIL_cpp
#define UTIL_cpp
#include <Util.h>
#include <Universal.h>

//************************************************************************
//************************************************************************
// This routine allows for the easy construction of TUN packets
// destined to be sent over SERIAL to the PC.
// Returns the packet size
uint8_t Util::create_TUN_serial_packet(	const uint8_t TUN_type,
											const uint8_t* str, 
											const uint8_t str_sz,
											uint8_t* buff,
											const uint8_t buff_sz)
{
	// the packet size
	uint8_t packet_sz = 0;

	if(str_sz > 0)
	{
		packet_sz = create_TUN_packet(	TUN_type, 
										str, 
										str_sz,
										buff, buff_sz);
	}

	return packet_sz;
}

//************************************************************************
//************************************************************************
// Constructs a complete TUN LCD packet.
// Returns:
//	packet_sz: size of complete TUN packet
//	buff: the complete TUN packet including the $%.
uint8_t Util::create_TUN_lcd_packet(	const boolean is_local,
										const uint8_t x, 
										const uint8_t y, 
										const uint8_t* str, 
										const uint8_t str_sz,
										uint8_t* buff,
										const uint8_t buff_sz)
{
	uint8_t packet_sz = 0;
	uint8_t payload_sz = 0;
	uint8_t payload_buff[MED_BUFF_SZ];
	
	// derive the packet_type
	uint8_t packet_type = 0;
	
	if(is_local)
		packet_type = TUN_TYPE_LOCAL_LCD_MSG;
	else
		packet_type = TUN_TYPE_EXTERNAL_LCD_MSG;
	
	
	// first, construct the payload
	payload_sz = construct_lcd_payload(	x, y, str, str_sz, 
										payload_buff, MED_BUFF_SZ);

	// second, create the complete packet
	if(payload_sz > 0)
	{
		packet_sz = create_TUN_packet(		packet_type, 
											payload_buff, 
											payload_sz,
											buff, buff_sz);
	}
	
	return packet_sz;
}

//************************************************************************
//************************************************************************
// Constructs a LCD payload. NOTE: This NOT a packet, simply the payload
// which must be used to create a full TUN packet.
// This routine should not be called directly. 
uint8_t Util::construct_lcd_payload(	const uint8_t x, 
										const uint8_t y, 
										const uint8_t* str, 
										const uint8_t str_sz,
										uint8_t* buff,
										const uint8_t buff_sz)
{
	// storage for total size
	uint8_t total_sz = 0;
		
	// clean given buffer
	clean_packet(buff, buff_sz);

	// tmp storage for conversions
	uint8_t conv_buff[SMALL_BUFF_SZ];
	clean_packet(conv_buff, SMALL_BUFF_SZ);
	
	// index into buff
	uint8_t buff_index = 0;
	
	// first calculate the size and ensure the 
	// caller supplied a big enough buffer
	total_sz += MISC_8BIT_HEX_SZ;
	total_sz += MISC_8BIT_HEX_SZ;
	total_sz += strlen((const char*)str);
	
	// only continue if buff size is big enough
	if(total_sz < (buff_sz-1))
	{
		// convert the x and store it
		int_to_hex(x, MISC_8BIT_HEX_SZ, (char*)conv_buff);
		buff[buff_index++] = conv_buff[0];
		buff[buff_index++] = conv_buff[1];
		
		// convert the y and store it
		int_to_hex(y, MISC_8BIT_HEX_SZ, (char*)conv_buff);
		buff[buff_index++] = conv_buff[0];
		buff[buff_index++] = conv_buff[1];
		
		// store the rest of the string
		for(uint8_t i=0; i < str_sz; i++)
			buff[buff_index++] = str[i];
	}
	else
		total_sz = 0;
	
	return total_sz;
}

//************************************************************************
//************************************************************************
// Removes the TUN frame if one exists
// Returns TRUE if frame was removed
// Returns FALSE if no frame was present
boolean Util::remove_TUN_frame(uint8_t* packet)
{
	boolean frame_removed = false;
	uint8_t packet_sz = 0;
	
	if(is_TUN_packet_framed(packet))
	{
		// first, wipe out the SENT_END_BYTE
		packet_sz = get_TUN_packet_sz(packet);
		packet[packet_sz-1] = SENT_CHAR_NULL_BYTE;
		
		// secondly, left-shift the entire packet
		for(uint8_t i = 0; i < packet_sz; i++)
			packet[i] = packet[i+1];

		// third, indicate that a frame has been removed
		frame_removed = true;
	}

	return frame_removed;
}

//************************************************************************
//************************************************************************
// Determines if a TUN packet is framed. A framed TUN packet has this 
// format:
// $[PACKET]%
// Basically, this function will return true if $% are found and false if
// any of the $% are missing.
boolean Util::is_TUN_packet_framed(const uint8_t* packet)
{
	uint8_t packet_sz = 0;
	boolean is_framed = false;

	// first get the packet size
	packet_sz = get_TUN_packet_sz(packet);
	
	// determine if the packet is properly framed
	if ( 	(packet[0] == SENT_START_BYTE) && 
			(packet[packet_sz-1] ==  SENT_END_BYTE) )
	{
		is_framed = true;
	}
	
	return is_framed;
}

//************************************************************************
//************************************************************************
// Use this function to count the non-null bytes in a TUN packet
// NOTE: This doesn't count the payload size. This routine counts
// the size of the ENTIRE TUN packet. This routine relies on the
// null byte as a stopping point. Therefore, there CANNOT be any 
// null-bytes within the TUN packet.
// We will assume that the largest possible packet
// is LARGE_BUFF_SZ. But do not rely on this.

uint8_t Util::get_TUN_packet_sz(const uint8_t* packet)
{
	// kick out of the loop upon hitting
	// the null byte ('\0') (SENT_CHAR_NULL_BYTE)
	boolean hit_null = false;
	
	// the count
	uint8_t cnt = 0;
	
	// index into packet
	uint8_t index = 0;

	while ((hit_null == false) && (index < LARGE_BUFF_SZ) )
	{
		if( packet[index++] != SENT_CHAR_NULL_BYTE )
			cnt++;
		else
			hit_null = true;
	}

	// return the count
	return cnt;
}

//************************************************************************
//************************************************************************
// Returns TRUE if checksum is OK, FALSE otherwise
boolean Util::verify_checksum(const uint8_t* packet)
{
	// for the stored checksum
	uint16_t stored_checksum = 0;
	
	// for the derived checksum
	uint16_t derived_checksum = 0;
	
	// get the values
	stored_checksum = get_TUN_checksum(packet);
	derived_checksum = derive_TUN_checksum(packet);

	// true or false
	if(stored_checksum == derived_checksum)
		return true;
	else
		return false;
}

//************************************************************************
//************************************************************************
// This routine calculates the checksum from a completed TUN packet. The purpose of
// this routine is not to calculate a new checksum to place within the TUN packet, but 
// rather derive the checksum from a completed packet to perform a checksum test
// on a TUN
// NOTE: This routine will only work on TUN packets that
// do NOT have the START and STOP bytes ($%)
uint16_t Util::derive_TUN_checksum(const uint8_t* packet)
{
	uint16_t derived_checksum = 0;
	
	// First get the payload size
	uint8_t TUN_payload_sz = get_TUN_payload_sz(packet);

	// add the first 8 bytes
	for(uint8_t i = 0; i < MISC_TUN_HEADER_NO_CHECKSUM_SZ; i++)
		derived_checksum += packet[i];
	
	// add the payload
	for(uint8_t i = TUN_PAYLOAD_START; i < (TUN_PAYLOAD_START + TUN_payload_sz); i++)
		derived_checksum += packet[i];
		
	// return the result
	return derived_checksum;
}

//************************************************************************
//************************************************************************
// Gets the TUN payload
// returns: the payload size
uint8_t Util::get_TUN_payload(const uint8_t* packet,
								uint8_t* buff, uint8_t buff_sz)
{
	uint8_t payload_sz = get_TUN_payload_sz(packet);

	// clean the buffer
	clean_packet(buff, buff_sz);
	
	// ensure the payload size is small enough to fit
	// into the buffer
	if(payload_sz < (buff_sz-1))
	{
		for(uint8_t i=0; i < payload_sz; i++)
		{
			buff[i] = packet[i + TUN_PAYLOAD_START];
		}
	}
	else
		payload_sz = 0;
								
	return payload_sz;		
}

//************************************************************************
//************************************************************************
// Returns the TUN checksum.
uint16_t Util::get_TUN_checksum(const uint8_t* packet)
{
	uint16_t checksum = 0;
	
	checksum = hex_to_int(	TUN_CHECKSUM_START, 
							TUN_CHECKSUM_END, 
							TUN_CHECKSUM_SZ, 
							packet);

	return checksum;
}

//************************************************************************
//************************************************************************
// Returns the TUN payload_sz.
uint8_t Util::get_TUN_payload_sz(const uint8_t* packet)
{
	uint8_t payload_sz = 0;
	
	payload_sz = hex_to_int(	TUN_PAYLOAD_SZ_START, 
								TUN_PAYLOAD_SZ_END, 
								TUN_PAYLOAD_SZ_SZ, 
								packet);

	return payload_sz;
}

//************************************************************************
//************************************************************************
// Returns the TUN type.
uint8_t Util::get_TUN_type(const uint8_t* packet)
{
	uint8_t type = 0;
	
	type = hex_to_int(	TUN_TYPE_START, 
						TUN_TYPE_END, 
						TUN_TYPE_SZ, 
						packet);

	return type;
}

//************************************************************************
//************************************************************************
// Copies a buffer
uint8_t Util::cpy_buff(uint8_t* dest, const uint8_t* src, const uint8_t byte_cnt)
{
	for(uint8_t i = 0; i < byte_cnt; i++)
		dest[i] = src[i];

	return byte_cnt;
}

//************************************************************************
//************************************************************************
// This function allows for the creation of a correctly formed
// tunnelled (TUN) packet. This packet will be the payload of the
// system (SYS) packet of type 0x10. When the XBee transmitts this packet,
// the recieving device will get a SYS packet of type 0x90. At this point,
// the reciever must extract the TUN packet from the SYS packet.
// Yes, the packet is tunnelled via 0x10 to 0x90.
// Incoming arguments:
// packet_type: The packet type of the TUN packet.
// payload: The actual raw payload of the TUN packet.
// payload_sz: the size of the incoming payload.
// buff: A buffer to store the completed TUN packet.
// buff_sz: the size of the completed TUN packet.
// Returns:
// total_sz: The total size of the TUN packet including all
// fields, etc.
//
// Format of the TUNNELED (TUN) packet in ASCII-HEX:
// $[TYPE:2][PAYLOAD_SZ:2][CHECKSUM:4][PAYLOAD:?]%


uint8_t Util::create_TUN_packet(	uint8_t packet_type, 
									const uint8_t* payload, 
									uint8_t payload_sz,
									uint8_t* buff, 
									uint8_t buff_sz)
{
	uint8_t total_sz = 0;
	uint16_t checksum = 0;
	uint8_t index = 0;
	
	// the work buffer
	uint8_t tmp_buff[SMALL_BUFF_SZ];
	
	// ensure the incoming buffer is large enough
	// to hold the completed TUN packet
	total_sz += MISC_TUN_HEADER_SZ; // packet header
	total_sz += MISC_TUN_START_STOP_SZ; // start and end bytes
	total_sz += payload_sz; // the size of the incoming payload

	// ensure the storage buffer has enough room
	if( total_sz < (buff_sz-1))
	{
		// get the checksum of the payload
		for(uint8_t i = 0; i < payload_sz; i++)
			checksum += payload[i];
	
		// clean the tmp buffer
		clean_packet(tmp_buff, SMALL_BUFF_SZ);
		
		// clean the buff
		clean_packet(buff, buff_sz);
		
		// place the start byte
		buff[index++] = SENT_START_BYTE;
		
		// convert the TUN packet type and store it
		int_to_hex(packet_type, MISC_8BIT_HEX_SZ, (char*)tmp_buff);
		buff[index++] = tmp_buff[0];
		buff[index++] = tmp_buff[1];
		checksum += tmp_buff[0];
		checksum += tmp_buff[1];
		clean_packet(tmp_buff, SMALL_BUFF_SZ);
		
		// convert the TUN payload size and store it
		int_to_hex(payload_sz, MISC_8BIT_HEX_SZ, (char*)tmp_buff);
		buff[index++] = tmp_buff[0];
		buff[index++] = tmp_buff[1];
		checksum += tmp_buff[0];
		checksum += tmp_buff[1];
		clean_packet(tmp_buff, SMALL_BUFF_SZ);
		
		// convert the checksum and store it
		int_to_hex(checksum, MISC_16BIT_HEX_SZ, (char*)tmp_buff);
		buff[index++] = tmp_buff[0];
		buff[index++] = tmp_buff[1];
		buff[index++] = tmp_buff[2];
		buff[index++] = tmp_buff[3];
		
		// place the payload into the packet
		for(uint8_t i = 0; i < payload_sz; i++)
			buff[index++] = payload[i];
			
		// place the end byte
		buff[index++] = SENT_END_BYTE;
	
	}
	else
	{
		// the total size of the packet
		// is larger than the buffer
		total_sz = 0;
	}
			
	return total_sz;			
}
		
//************************************************************************
//************************************************************************
// Helper routine to get the SIZE of the packet
uint8_t Util::get_SYS_packet_length(const uint8_t* packet)
{
	uint8_t length = SENT_API_NULL_BYTE;
	
	// storage for the packet size bytes
	uint8_t bytes[SMALL_BUFF_SZ];
	
	// extraction of the bytes
	bytes[0] = packet[SLOC_API_LENGTH_MSB_LOC];
	bytes[1] = packet[SLOC_API_LENGTH_LSB_LOC];
	
	length = word(bytes[0], bytes[1]);
	
	return length;
}

//************************************************************************
//************************************************************************
// cleans the current packet
void Util::clean_packet(uint8_t* packet, uint16_t size)
{
	for(uint16_t i = 0; i < size; i++)
		packet[i] = SENT_API_NULL_BYTE;
}

//************************************************************************
//************************************************************************
// Convert hex into integer
// The buffer is assumed to be HEX ASCII, not byte based
uint16_t Util::hex_to_int(int start_byte, int end_byte, int size, const uint8_t* buffer)
{
	uint16_t result = 0;

	// storage of tmp hex buffer
	uint8_t hex_buf[SMALL_BUFF_SZ];
	
	// protection
	if(size < SMALL_BUFF_SZ)
	{
		// keep track of buffer index
		uint8_t hex_index = 0;
	
		for(uint8_t i = start_byte; i <= end_byte; i++)
		{
			hex_buf[hex_index++] = buffer[i];
		}
	
		hex_buf[hex_index] = SENT_CHAR_NULL_BYTE;
		result = (uint16_t)strtol((char*)hex_buf, NULL, 16);
	}
	
	return result;
}

//************************************************************************
//************************************************************************
// Convert ascii hex byte to int
uint8_t Util::ascii_hex_byte_to_int(const uint8_t msb, const uint8_t lsb)
{
	uint8_t result = RET_ZERO;
	
	// create a buffer to send to the routine
	uint8_t buff[SMALL_BUFF_SZ];
	
	// copy over the bytes
	buff[0] = msb;
	buff[1] = lsb;
	buff[2] = SENT_CHAR_NULL_BYTE;
	
	// call the normal routine
	result = (uint8_t)hex_to_int(0, 1, 2, buff);
	
	return result;
}

//************************************************************************
//************************************************************************
// Converts an integer into a hex string
// size is the total size allocated for the hex. If the hex result
// doesn't take up the entire space, 0's are used to fill in the blanks
// set the stor_buff to have size MED_BUFF_SZ
void Util::int_to_hex(const int value, const int size, char stor_buff[])
{
	char scr_buffer[SMALL_BUFF_SZ];
	char scr2_buffer[SMALL_BUFF_SZ];
	
	// construct the correct conversion string
	scr_buffer[0] = '%';
	scr_buffer[1] = '0';
	scr_buffer[2] = SENT_CHAR_NULL_BYTE;
	itoa(size, scr2_buffer, 10);
	strcat(scr_buffer, scr2_buffer);
	strcat(scr_buffer, "X");
	sprintf (stor_buff, scr_buffer, value);
}

//************************************************************************
//************************************************************************
// Converts an integer into a hex string
// size is the total size allocated for the hex. If the hex result
// doesn't take up the entire space, 0's are used to fill in the blanks
// set the stor_buff to have size MED_BUFF_SZ
void Util::int_to_hex32(uint32_t value, uint32_t size, char stor_buff[])
{
	char scr_buffer[SMALL_BUFF_SZ];
	char scr2_buffer[SMALL_BUFF_SZ];
	
	// construct the correct conversion string
	scr_buffer[0] = '%';
	scr_buffer[1] = '0';
	scr_buffer[2] = SENT_CHAR_NULL_BYTE;
	itoa(size, scr2_buffer, 10);
	strcat(scr_buffer, scr2_buffer);
	strcat(scr_buffer, "lX");
	snprintf(stor_buff, MED_BUFF_SZ, scr_buffer, value);
}

//************************************************************************
//************************************************************************
// Constructor
Util::Util()
{

}


#endif 