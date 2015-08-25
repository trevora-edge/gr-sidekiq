/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
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
#include "sidekiq_sink_s_impl.h"

#define     SIDEKIQ_SAMPLES_PER_PACKET        (1024)
// *2 for I & Q
#define BUF_LEN      (SIDEKIQ_SAMPLES_PER_PACKET*sizeof(short)*2)

namespace gr {
  namespace sidekiq {

      sidekiq_sink_s::sptr
      sidekiq_sink_s::make(const std::string ip_address, uint32_t port)
      {
	  return gnuradio::get_initial_sptr
	      (new sidekiq_sink_s_impl(ip_address, port));
      }

      /*
       * The private constructor
       */
      sidekiq_sink_s_impl::sidekiq_sink_s_impl(const std::string ip_address,
                                               uint32_t port)
	  : gr::sync_block("sidekiq_tx",
			   gr::io_signature::make(1, 1, sizeof(short)),
                           gr::io_signature::make(0, 0, 0))
      {
	  tx.reset( new sidekiq_tx(ip_address.c_str(), port) );

          // TODO: add this in!!...probably should be block size
	  //set_input_multiple(SIDEKIQ_SAMPLES_PER_PACKET*2);

          std::stringstream str;
          str << name() << "_" << unique_id();
          _id = pmt::string_to_symbol(str.str());

	  //rcv.reset( new sidekiq_sink(ip_address.c_str(), port) );
	  //set_input_multiple(SIDEKIQ_SAMPLES_PER_PACKET*2);
      }

      /*
       * Our virtual destructor.
       */
      sidekiq_sink_s_impl::~sidekiq_sink_s_impl()
      {
      }
      
      uint64_t 
      sidekiq_sink_s_impl::set_center_freq(uint64_t freq)
      {
	  return (tx->set_center_freq(freq));
      }

      uint64_t 
      sidekiq_sink_s_impl::set_center_freq(float freq)
      {
	  return (tx->set_center_freq(freq));
      }

      uint64_t 
      sidekiq_sink_s_impl::center_freq(void)
      {
	  return (tx->center_freq());
      }

      int
      sidekiq_sink_s_impl::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
      {
          int ninput_items = noutput_items;
          const int16_t* in = reinterpret_cast<const int16_t *>(input_items[0]);

          //printf("input %d\r\n", ninput_items);

#if 0
          // TODO: super dumb but it won't compile...
          printf("copying...\r\n");
          for( uint32_t i=0; i<ninput_items; i++ )
          {
              printf("i is %u\r\n", i);
              data[(i*2)] = (int16_t)(output_items[i]);
              data[(i*2)+1] = *(int16_t*)(output_items[i]);
              printf("data %d %d\r\n", data[(i*2)], data[(i*2)+1]);
          }
#endif
          //printf("done copying\r\n");
          tx->transmit( (const int16_t*)(in), ninput_items );
          //printf("transmit done\r\n");

          return (ninput_items);
      }

  } /* namespace sidekiq */
} /* namespace gr */

