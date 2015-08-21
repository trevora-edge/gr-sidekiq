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
	  //return (rcv->set_center_freq(freq));
          return (0);
      }

      uint64_t 
      sidekiq_sink_s_impl::set_center_freq(float freq)
      {
	  //return (rcv->set_center_freq(freq));
          return (0);
      }

      uint64_t 
      sidekiq_sink_s_impl::center_freq(void)
      {
	  //return (rcv->center_freq());
          return (0);
      }

      int
      sidekiq_sink_s_impl::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
      {
          // TODO
          return (0);
      }

  } /* namespace sidekiq */
} /* namespace gr */

