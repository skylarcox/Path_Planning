
#ifndef SINGLE_BUFF_cpp
#define SINGLE_BUFF_cpp
#include <single_buff.h>


//***************************************************
//***************************************************
// Returns the current buffer ID
uint16_t Single_buff::get_buffer_id()
{
	return m_buffer_id;
}

//***************************************************
//***************************************************
// Gets the TUN type
// Returns: -1 or type
int Single_buff::get_TUN_type()
{
	int type = -1;
	
	if(any_items())
		type = m_util.get_TUN_type(m_buff_array);

	return type;
}

//***************************************************
//***************************************************
// Resets the buffer
void Single_buff::reset_buffer()
{
	m_buff_ready = false;
	m_buff_sz = 0;
	m_util.clean_packet(m_buff_array, LARGE_BUFF_SZ);
}

//***************************************************
//***************************************************
// Returns true if there are any items in the stack
// Returns false if there is not
boolean Single_buff::any_items()
{
	return m_buff_ready;
}

//***************************************************
//***************************************************
// Add a buffer ans set the buffer ID
uint8_t Single_buff::add_TUN_buffer(uint8_t* buff, const uint8_t sz, const uint8_t ID)
{
	uint8_t buff_sz = 0;

	// set the buffer
	buff_sz = add_TUN_buffer(buff, sz);
	
	if( sz > 0)
	{
		// set the ID
		m_buffer_id = ID;
	}
	
	return buff_sz;
}

//***************************************************
//***************************************************
// Add a SYS buffer
uint8_t Single_buff::add_SYS_buffer(uint8_t* buff, const uint8_t sz)
{
	// ensure the size is less than max
	if(sz < LARGE_BUFF_SZ )
	{
		// reset the buffer
		reset_buffer();
		
		// copy over the buffer
		m_util.cpy_buff(m_buff_array, buff, sz);
	
		// signal that we have a buffer ready
		m_buff_ready = true;
		
		// update the packet_id 
		m_buffer_id++;
	}
	else 
		return 0;
		
	return m_buff_sz;
}


//***************************************************
//***************************************************
// Add a TUN buffer
uint8_t Single_buff::add_TUN_buffer(uint8_t* buff, const uint8_t sz)
{
	// keep track if we had to remove the frame
	boolean frame_removed = false;

	// ensure the size is less than max
	if(sz < LARGE_BUFF_SZ )
	{
		// reset the buffer
		reset_buffer();
	
		// remove the TUN framing just-in-case
		frame_removed = m_util.remove_TUN_frame(buff);
		
		// only add the TUN packet if it passes
		// the checksum
		if(m_util.verify_checksum(buff))
		{
			// set the size
			if(frame_removed)
				m_buff_sz = sz - 2;
			else
				m_buff_sz = sz;
		
			// copy over the buffer
			m_util.cpy_buff(m_buff_array, buff, m_buff_sz);
	
			// signal that we have a buffer ready
			m_buff_ready = true;
		
			// update the packet_id 
			m_buffer_id++;
		
		}
	}
	else 
		return 0;
		
	return m_buff_sz;
}

//***************************************************
//***************************************************
// Pops the top of the stack
uint8_t Single_buff::pop_top(uint8_t* buff, const uint8_t sz)
{
	// ensure there is even a top
	if(!m_buff_ready)
		return 0;
	
	// ensure the buffer to copy to is big enough
	if(sz < m_buff_sz)
		return 0;
	
	// copy over the buffer	
	m_util.cpy_buff(buff, m_buff_array, m_buff_sz);
		
	// buffer no longer ready
	reset_buffer();
	
	return m_buff_sz;
}

//***************************************************
//***************************************************
// constructor
Single_buff::Single_buff()
{
	reset_buffer();
	m_buffer_id = 0;
}

#endif