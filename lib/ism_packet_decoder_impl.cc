/* -*- c++ -*- */
/* 
 * Copyright 2017 <dpidhirney@seekintoo.com> Seekintoo Ltd.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "ism_packet_decoder_impl.h"

namespace gr {
  namespace ism {

    ism_packet_decoder::sptr
    ism_packet_decoder::make(msg_queue::sptr target_queue, bool do_crc16_check, bool verbose)
    {
      return gnuradio::get_initial_sptr
        (new ism_packet_decoder_impl(target_queue, do_crc16_check, verbose));
    }

    /*
     * The private constructor
     */
    ism_packet_decoder_impl::ism_packet_decoder_impl(msg_queue::sptr arg_target_queue, bool arg_do_crc16_check, bool arg_verbose)
      : gr::block("ism_packet_decoder",
              gr::io_signature::make(1, 1, sizeof(unsigned char)),
              gr::io_signature::make(1, 1, sizeof(unsigned char))),

              target_queue(arg_target_queue),
              do_crc16_check(arg_do_crc16_check),
              verbose(arg_verbose),

              is_msg(false),
              buffer_expected_len(0),
              bit_index(0),
              buffer_i(0)
    {
        // reset buffer
        buffer_reset();
    }

    /*
     * Our virtual destructor.
     */
    ism_packet_decoder_impl::~ism_packet_decoder_impl()
    {
    }

    void
    ism_packet_decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
	ninput_items_required[0] = noutput_items;
    }

    int
    ism_packet_decoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {

        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        unsigned int produced=0;

        // Do <+signal processing+>
        for(int i=0;i<noutput_items;i++){

                // we are currently processing a message
                if(is_msg){
                        // add new bit
                        buffer_append(in[i]);
                        // end of message ? 
                        if ((buffer_i != 0 && bit_index == 8 && buffer_i == buffer_expected_len - 1) ||
                                buffer_i == (BUF_MAX_SIZE)-1){
                                is_msg = false; // stop
                                buffer_flush(out);
                                produced = buffer_i+1;
                        }
                }else{
                        // did we find the beginning of a new message ?
                        if(in[i] & 0x02){
                                // reset buffer 
                                buffer_reset();
                                // add the first bit
                                buffer_append(in[i]);
                                // swith processing mode
                                is_msg = true;
                        }
                }
        }

        // Tell runtime system how many input items we consumed on
        // each input stream.
        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return produced;
    }



    /****************************************************************
    BUFFER FLUSH
    ****************************************************************/
    // print/send out current buffer
    int ism_packet_decoder_impl::buffer_flush(unsigned char* out){

    if(verbose){
        fprintf(stdout, "[verbose] Pkt: ");
                for(int j=0;j<buffer_i+1;j++){
                        fprintf(stdout, "%02x ", (unsigned char)buffer[j]);
                }
    }

    if(verbose){
        fprintf(stdout, "\n");
    }

        //char timestamp_buf[64];
	uint16_t checksum = CRC_INIT;
	uint16_t checksum_found;

	// CHECKSUM ?
	if(do_crc16_check){
		
                // get CRC from frame (2 last bytes)
                checksum_found = buffer[buffer_i-1];
                checksum_found <<= 8;
                checksum_found += buffer[buffer_i];
		// compute real CRC
                for(int i = 0; i < (buffer_i+1) - 2 ; i++)
                        checksum = culCalcCRC((unsigned char)buffer[i], checksum);
		// drop frame if wrong CRC
		if(checksum != checksum_found){
			if(verbose){
				fprintf(stdout, "[crc error]\n");
			}

			buffer_i = -1; // on return, we do 'produced = buffer_i+1;'
			return 0;
		}else{
			// if CRC is correct, remove it from frame
			buffer_i -= 2;
		}
	}

    // copy buffer to out port
    memcpy(out, buffer, buffer_i+1);
    // .. and send a message to the queue
    message::sptr msg = message::make(0, 0, 0, buffer_i+1);
    memcpy(msg->msg(), buffer, buffer_i+1);   // copy
    target_queue->insert_tail(msg);       // send it
    msg.reset();                          // free it up

	return 1;
    }


    /****************************************************************
    BUFFER APPEND
    ****************************************************************/
    // we receive a byte and add the first bit to the end of the buffer
    int ism_packet_decoder_impl::buffer_append(unsigned char byte){

        // need a new byte in buffer[] ?
        if(bit_index == 8){
                bit_index=0;
                buffer_i++;
        }

	// is the first byte complete ? If yes, it contains the length of the message
	// header len and CRC must be added
	if(buffer_i == 1 && bit_index == 0){
        buffer_expected_len =     (int)(buffer[0]);
        
		// add crc len ?
		if(do_crc16_check){
			buffer_expected_len +=  2; // CRC16 is of course 2 bytes length
		}
		// add header len
		buffer_expected_len += 1;  // header is 1 byte (len)
	}

        // add new bit to the end of the buffer
        if(bit_index==0){
                buffer[buffer_i] = (byte & 0x1);
        }else{
                buffer[buffer_i] = (buffer[buffer_i] << 1) | (byte & 0x1);
        }
        // inc bit index
        bit_index++;

        return 1;
    }

    /****************************************************************
    BUFFER RESET
    ****************************************************************/
    int ism_packet_decoder_impl::buffer_reset(){
	memset(buffer,0,BUF_MAX_SIZE);
        bit_index=0;
        buffer_i=0;
        return 1;
    }


   /****************************************************************
   CRC16
   ****************************************************************/
    uint16_t ism_packet_decoder_impl::culCalcCRC(unsigned char crcData, uint16_t crcReg) {
        unsigned int i;
        for (i = 0; i < 8; i++) {
                if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
                        crcReg = (crcReg << 1) ^ CRC16_POLY;
                else
                        crcReg = (crcReg << 1);
                crcData <<= 1;
        }
        return crcReg;
    }

  } /* namespace ism */
} /* namespace gr */

