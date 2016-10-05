
/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
 * 
b * This is free software; you can redistribute it and/or modify
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

#ifndef INCLUDED_SIDEKIQ_SINK_IMPL_H
#define INCLUDED_SIDEKIQ_SINK_IMPL_H

#include <sidekiq/sidekiq_source_s.h>
#include <sidekiq/sidekiq_defs.h>
#include "sidekiq_tx.h"

namespace gr {
  namespace sidekiq {

    class sidekiq_sink_s_impl;
    typedef boost::shared_ptr<sidekiq_sink_s_impl> sidekiq_sink_s_impl_sptr;

    class sidekiq_sink_s_impl : public sidekiq_sink_s
    {
    private:
	boost::scoped_ptr<sidekiq_tx> tx;
        pmt::pmt_t _id;

    public:
	sidekiq_sink_s_impl(const std::string ip_adress, uint32_t port);
	~sidekiq_sink_s_impl();

	uint64_t set_center_freq(uint64_t freq);
	uint64_t set_center_freq(float freq);
	uint64_t center_freq(void);

	uint32_t set_sample_rate(uint32_t sample_rate);
	uint32_t set_sample_rate(float sample_rate);
	uint32_t sample_rate(void);

	uint32_t set_bandwidth(uint32_t bandwidth);
	uint32_t set_bandwidth(float bandwidth);
	uint32_t bandwidth(void);

        uint16_t set_tx_attenuation(uint16_t attenuation);
        uint16_t tx_attenuation(void);

	// Where all the action really happens
	int work(int noutput_items,
		 gr_vector_const_void_star &input_items,
		 gr_vector_void_star &output_items);

#if 0
	bool stop();
	bool start();
#endif
    };

  } // namespace sidekiq
} // namespace gr

#endif /* INCLUDED_SIDEKIQ_SINK_IMPL_H */

