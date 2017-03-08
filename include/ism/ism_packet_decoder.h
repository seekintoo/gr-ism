/* -*- c++ -*- */
/* 
 * Copyright 2017 Seekintoo Ltd. 
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


#ifndef INCLUDED_ISM_ISM_PACKET_DECODER_H
#define INCLUDED_ISM_ISM_PACKET_DECODER_H

#include <ism/api.h>
#include <gnuradio/block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace ism {

    /*!
     * \brief <+description of block+>
     * \ingroup ism
     *
     */
    class ISM_API ism_packet_decoder : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<ism_packet_decoder> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ism::ism_packet_decoder.
       *
       * To avoid accidental use of raw pointers, ism::ism_packet_decoder's
       * constructor is in a private implementation
       * class. ism::ism_packet_decoder::make is the public interface for
       * creating new instances.
       */
      static sptr make(msg_queue::sptr target_queue, bool, bool);
    };

  } // namespace ism
} // namespace gr

#endif /* INCLUDED_ISM_ISM_PACKET_DECODER_H */

