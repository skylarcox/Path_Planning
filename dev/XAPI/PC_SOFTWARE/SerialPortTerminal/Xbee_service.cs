using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SerialPortTerminal
{
    class Xbee_service
    {
        // State info for temp buffer
        int m_TUN_buff_index = 0;
        Boolean m_TUN_start_found = false;
        Boolean m_TUN_end_found = false;
        byte[] m_TUN_tmp;
        
        // for long-term storage a completed
        // rx packet from the XBee hardware. 
        byte[] m_TUN_rx_packet;
        Boolean m_TUN_rx_packet_ready = false;
        int m_TUN_rx_packet_sz = 0;

        // for utlities
        Util m_util;

        //************************************************************************
        //************************************************************************
        // filter incoming bytes into a TUN packet
        public void TUN_filter_packet(byte[] b_array)
        {
            foreach (byte b in b_array)
                TUN_filter_packet(b);
        }

        //************************************************************************
        //************************************************************************
        // filter incoming bytes into a TUN packet
        public void TUN_filter_packet(string str_tmp)
        {
            byte[] str_byte_array;
            
            // get a byte array
            str_byte_array = m_util.GetBytes(str_tmp);

            // process the byte array
            foreach (byte b in str_byte_array)
            {
                TUN_filter_packet(b);
            }
        }

        //************************************************************************
        //************************************************************************
        // filter incoming bytes into a TUN packet
        public void TUN_filter_packet(char tx_byte)
        {
            byte c = (byte)tx_byte;
            TUN_filter_packet(c);
        }

        //************************************************************************
        //************************************************************************
        // filter incoming bytes into a TUN packet
        // returns 0 on NO error
        // returns MISC_CHECKSUM_ERROR on checksum error
        public int TUN_filter_packet(byte tx_byte)
        {
            int result = 0;


            if (tx_byte == (byte)SENT_values.SENT_START_BYTE)
            {
                // we got a new temp packet
                TUN_reset_tmp_buffer();
                m_TUN_start_found = true;
            }
            else if (tx_byte == (byte)SENT_values.SENT_END_BYTE)
            {
                // ensure we have read start byte
                // and there is more than 0 characters in buffer
                if ((m_TUN_start_found == true) && (m_TUN_buff_index > 0))
                {
                    // we have a complete packet
                    m_TUN_end_found = true;

                    // store the packet
                    if (m_util.verify_packet(m_TUN_tmp))
                    {
                        store_tmp_packet_into_rx_packet(m_TUN_tmp, m_TUN_buff_index);
                    }
                    else
                    {
                        TUN_reset_tmp_buffer();
                        result = (int)MISC_values.CHECKSUM_ERROR;
                    }
                }
            }
            else if(    (m_TUN_start_found == true) &&
				        (m_TUN_end_found == false) &&
				        (m_TUN_buff_index < ((int)BUFF_sizes.LARGE_BUFF_SZ - 1)) )
			{
                // we have a single character that is
                // not a stop or a start
                // So just store it
                m_TUN_tmp[m_TUN_buff_index++] = tx_byte;
            }
            return result;
        }

        //************************************************************************
        //************************************************************************
        // Extracts the completed RX packet.
        // Returns:
        // The size of the RX packet
        public int get_rx_packet(out byte[] packet)
        {
            packet = new byte[(int)BUFF_sizes.LARGE_BUFF_SZ];
            int packet_sz = 0;

            // ensure the packet is ready
            if (is_rx_packet_ready())
            {
                // copy over the packet
                Array.Copy(m_TUN_rx_packet, packet, m_TUN_rx_packet_sz);

                // capture the size
                packet_sz = m_TUN_rx_packet_sz;

                // clear out the packet
                TUN_reset_rx_packet();
            }
            return packet_sz;
        }

        //************************************************************************
        //************************************************************************
        // return true or false if a rx completed packet is ready
        public Boolean is_rx_packet_ready()
        {
            return m_TUN_rx_packet_ready;
        }

        //************************************************************************
        //************************************************************************
        // Store the completed packet
        private void store_tmp_packet_into_rx_packet(byte[] tmp_packet, int tmp_sz)
        {
            // reset the rx packet
            TUN_reset_rx_packet();

            // copy all the stuff over
            Array.Copy(tmp_packet, m_TUN_rx_packet, tmp_sz);
            m_TUN_rx_packet_ready = true;
            m_TUN_rx_packet_sz = tmp_sz;

            // reset the tmp packet
            TUN_reset_tmp_buffer();
        }

        //************************************************************************
        //************************************************************************
        // reset the rx packet
        private void TUN_reset_rx_packet()
        {
            m_TUN_rx_packet = new byte[(int)BUFF_sizes.LARGE_BUFF_SZ];
            m_TUN_rx_packet_ready = false;
            m_TUN_rx_packet_sz = 0;
        }

        //************************************************************************
        //************************************************************************
        // reset the state and packet
        private void TUN_reset_tmp_buffer()
        {
            m_TUN_buff_index = 0;
	        m_TUN_start_found = false;
	        m_TUN_end_found = false;
            m_TUN_tmp = new byte[(int)BUFF_sizes.LARGE_BUFF_SZ];
        }

        //************************************************************************
        //************************************************************************
        // Constructor
        public Xbee_service()
        {
            m_util = new Util();
            TUN_reset_tmp_buffer();
            TUN_reset_rx_packet();
        }
    }
}
