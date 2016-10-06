/* -*- c++ -*- */
/* 
 * Copyright 2015 Epiq Solutions.
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
#include <config.h>
#endif
#include "sidekiq_tx.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#include <netdb.h>
#include <string>
#include <math.h>
#include <stdexcept>
#include <inttypes.h>
#include "srfs_interface.h"

namespace gr {
    namespace sidekiq {

#define DEBUG_SIDEKIQ 1
#define DEBUG(A)    if( DEBUG_SIDEKIQ ) printf("=debug=> %s\n", A)

#define FREQUENCY_MIN  47000000ULL
#define FREQUENCY_MAX 6000000000ULL
#define FREQUENCY_RESOLUTION 1

#define SAMPLE_RATE_MIN   233000
#define SAMPLE_RATE_MAX 50000000
#define SAMPLE_RATE_RESOLUTION 1

#define BANDWIDTH_MIN   233000
#define BANDWIDTH_MAX 50000000
#define BANDWIDTH_RESOLUTION 1
 
#define ATTENUATION_MIN   0
#define ATTENUATION_MAX 359
#define ATTENUATION_RESOLUTION 1
       
#define BLOCK_SIZE (8188) // hard coded for now         

sidekiq_tx::sidekiq_tx(const char *ip_addr, unsigned short port)
{
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    char msg[1000];
    

    d_server = gethostbyname( ip_addr );

    if ((d_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	throw std::runtime_error("unable to create socket");
    }

    if( setsockopt( d_sock, SOL_SOCKET, SO_RCVTIMEO, 
		    (char*)(&tv), sizeof(struct timeval)) != 0 )
    {
	throw std::runtime_error("unable to set socket options");
    }

    memset( &d_server_addr, 0, sizeof(d_server_addr) );
    d_server_addr.sin_family = AF_INET;
    d_server_addr.sin_port = htons(port);
    d_server_addr.sin_addr.s_addr = INADDR_ANY;
    memcpy( &d_server_addr.sin_addr.s_addr,
            d_server->h_addr,
            d_server->h_length );

    if ( 0 != connect( d_sock,
                       (struct sockaddr *)&d_server_addr,
                       sizeof( d_server_addr ) ) ) {
	throw std::runtime_error("unable to connect to sidekiq");
    }
    printf("!!!!got connection!!!!\r\n");

    d_addr_len = sizeof(struct sockaddr);

    d_srfs_src_status = STATUS_DISABLED;

    // TODO: default params
    d_tx_freq = 800000000;
    d_tx_sample_rate = 10000000;
    d_tx_bandwidth = d_tx_sample_rate;
    d_tx_atten = 50;

    printf("TX INIT SRFS\r\n");
    init_srfs_params();

    printf("TX OPEN SRFS\r\n");

    // Get SRFS spun up
    open_srfs();
    
    printf("!!!!!!!!!!!!!!!!!calling start!!!!!!!!!!!!!\r\n");
    start();
}

sidekiq_tx::~sidekiq_tx()
{
    stop();
    close_srfs();
}

void
sidekiq_tx::init_srfs_params(void)
{
    // frequency
    add_srfs_param( "frequency",
		    srfs::SRFS_UINT64,
		    (void*)(&d_tx_freq),
		    FREQUENCY_MIN,
		    FREQUENCY_MAX,
		    FREQUENCY_RESOLUTION,
		    NULL );

    // sample rate
    add_srfs_param( "sample_rate",
		    srfs::SRFS_UINT32,
		    (void*)(&d_tx_sample_rate),
		    SAMPLE_RATE_MIN,
		    SAMPLE_RATE_MAX,
		    SAMPLE_RATE_RESOLUTION,
		    NULL );

    // bandwidth
    add_srfs_param( "bandwidth",
		    srfs::SRFS_UINT32,
		    (void*)(&d_tx_bandwidth),
		    BANDWIDTH_MIN,
		    BANDWIDTH_MAX,
		    BANDWIDTH_RESOLUTION,
		    NULL );

    // bandwidth
    add_srfs_param( "attenuation",
		    srfs::SRFS_UINT16,
		    (void*)(&d_tx_atten),
		    ATTENUATION_MIN,
		    ATTENUATION_MAX,
		    ATTENUATION_RESOLUTION,
		    NULL );
    
#if 0

    // rx gain
    add_srfs_param( "rx_gain",
		    srfs::SRFS_UINT8,
		    (void*)(&d_rx_gain),
		    RX_GAIN_MIN,
		    RX_GAIN_MAX,
		    RX_GAIN_RESOLUTION,
		    NULL );

    // gain mode
    add_srfs_param( "gain_mode",
		    srfs::SRFS_ENUM,
		    (void*)(&d_rx_gain_mode),
		    0, 
		    GAIN_MODE_INVALID,
		    1,
		    gain_mode_str );
#endif
}

void 
sidekiq_tx::add_srfs_param( const std::string token,
                            srfs::SRFS_DATATYPES data_type,
                            void *p_value,
                            int64_t min_value,
                            int64_t max_value,
                            float resolution,
                            const std::string *p_strings )
{
    srfs::srfs_param_t param;

    param.data_type = data_type;
    param.p_value = p_value;
    param.min_value = min_value;
    param.max_value = max_value;
    param.resolution = resolution;
    param.p_strings = p_strings;

    sidekiq_params[token] = param;
}

void
sidekiq_tx::set_param( const std::string token, void *pValue )
{
    param_map::iterator iter;

    if( d_state == STATE_STARTED ) {
	iter = sidekiq_params.find(token);
	if( iter != sidekiq_params.end() ) {
	    if( srfs::set_param(&(iter->second), pValue) ) {
		config_src();
	    }
	}
	else {
	    throw std::invalid_argument("unknown parameter specified");
	}
    }
    else {
	throw std::runtime_error("no sidekiq connection when trying to set param");
    }
}

uint64_t 
sidekiq_tx::set_center_freq(uint64_t freq)
{
    set_param( "frequency", &freq );
    return d_tx_freq;
}

uint64_t 
sidekiq_tx::center_freq(void)
{
    return d_tx_freq;
}

uint32_t 
sidekiq_tx::set_sample_rate(uint32_t sample_rate)
{
    set_param("sample_rate", &sample_rate);
    return d_tx_sample_rate;
}
 
uint32_t
sidekiq_tx::sample_rate(void)
{
    return d_tx_sample_rate;
}

uint32_t 
sidekiq_tx::set_bandwidth(uint32_t bandwidth)
{
    set_param("bandwidth", &bandwidth);
    d_tx_bandwidth = bandwidth;
}

uint32_t 
sidekiq_tx::bandwidth(void)
{
    return d_tx_bandwidth;
}

uint16_t 
sidekiq_tx::set_tx_attenuation(uint16_t attenuation)
{
    set_param("attenuation", &attenuation);
    d_tx_atten = attenuation;
}

uint16_t 
sidekiq_tx::tx_attenuation(void)
{
    return d_tx_atten;
}

void 
sidekiq_tx::open_srfs()
{
    char cmd[1024];
    char rcv[1024];

    std::string str1;
    std::string str2;
    std::string str3;

#if 0
    // reset to the default state
    snprintf(cmd, 1024, "reset!\n");
    send_msg( cmd );
    receive_msg( rcv, 1024 ); // clear out rcv buf
#else
    printf("!!!!!!!!!!!TODO: coordinate reset!!!!\r\n");
#endif

    // subscribe to SIDEKIQ-TX
    snprintf(cmd, 1024, "subscribe! block SIDEKIQ-TX\n");
    send_msg( cmd );
    receive_msg( rcv, 1024 );
    
    // parse out source port
    str1 = std::string( rcv );
    str2 = str1.substr( str1.find(":") + 1 ); // Move one past :
    str3 = str2.substr( 0, str2.find(" ") ); // src_port is between : and space
    d_src_port = atoi( str3.c_str() );

    d_state = STATE_STARTED;
    d_srfs_src_status = STATUS_ENABLED;
}

void
sidekiq_tx::transmit( const int16_t *p_data, uint16_t num_samples )
{
    int flags = 0;
    uint32_t num_samples_sent=0;
    uint32_t sendCount=0;
    
    static int16_t samples_with_hdr[(BLOCK_SIZE*2) + 8]; // +8 for header
    static uint32_t dataIndex = BLOCK_SIZE*2;

transfer_and_send_data:    
    if( dataIndex >= (BLOCK_SIZE*2) )
    {
        // reset it back to 0
        dataIndex = 0;
        //num_pairs = 0;

        //printf("after reset...%u %u %u\r\n", num_pairs, num_samples, dataIndex);
    }

    // copy over the data
    for( dataIndex; dataIndex < (BLOCK_SIZE*2); dataIndex++ )
    {
        // see if we have more data still
        if( num_samples_sent >= num_samples )
        {
            break;
        }

        samples_with_hdr[8 + dataIndex] = p_data[num_samples_sent];
        num_samples_sent++;
    }


    // send a block if we have a full one
    if( dataIndex >= (BLOCK_SIZE*2) )
    {
        if( (sendCount=send( d_iq_sock, samples_with_hdr, ((BLOCK_SIZE*2+8)*2), flags )) < 0 )
        {
            printf("send fail\r\n");
        }
    }
    // see if there's more data to copy off
    if( (num_samples_sent) < num_samples )
    {
        goto transfer_and_send_data;
    }
}


void 
sidekiq_tx::start()
{
    char cmd[1024];
    char rcv[1024];

    if( DEBUG_SIDEKIQ ) { 
	printf("starting\n");
    }

    // make sure it's configured
    config_src();

    // enable the IQ data
    snprintf( cmd, 1024,
             "config! block SIDEKIQ-TX:%d status enabled\n",
              d_iq_port );
    send_msg(cmd);
    receive_msg(rcv, 1024);

    d_iq_sock = socket( AF_INET, SOCK_STREAM, 0 );

    memset( &d_iq_server_addr, 0, sizeof(d_iq_server_addr) );
    d_iq_server_addr.sin_family = AF_INET;
    d_iq_server_addr.sin_port = htons( 7001 ); // TODO
    memcpy( &d_iq_server_addr.sin_addr.s_addr,
        d_server->h_addr,
        d_server->h_length );

    printf("!!!!!!!!!trying to connect IQ socket!!!!!\r\n");

    if ( 0 != connect( d_iq_sock,
                       (struct sockaddr *)&d_iq_server_addr,
                       sizeof(d_iq_server_addr) ) ) {
        throw std::runtime_error("unable to connect to IQ socket");
    }

    printf("IQ socket ok\r\n");
// TODO
#if 0
    //###################################
    //#     Now Open new IQ Port        #
    //###################################
    d_iq_sock = socket( AF_INET, SOCK_STREAM, 0 );

    memset( &d_iq_server_addr, 0, sizeof(d_iq_server_addr) );
    d_iq_server_addr.sin_family = AF_INET;
    d_iq_server_addr.sin_port = htons( d_iq_port );
    memcpy( &d_iq_server_addr.sin_addr.s_addr,
        d_server->h_addr,
        d_server->h_length );

    if ( 0 != connect( d_iq_sock,
                       (struct sockaddr *)&d_iq_server_addr,
                       sizeof(d_iq_server_addr) ) ) {
	throw std::runtime_error("unable to connect to IQ socket");
    }

    // enable the IQ data
    snprintf( cmd, 1024,
	     "config! block IQ:%d status enabled\n",
	      d_iq_port );
    send_msg(cmd);
    receive_msg(rcv, 1024);

    first = true;
    
    if( DEBUG_SIDEKIQ ) {
	printf("Successfully opened both ports\n");
    }
#endif
}

void 
sidekiq_tx::close_srfs()
{
    char cmd[1024];
    char rcv[1024];

    if ( d_state == STATE_STARTED ) {

	// unsubscribe from the RX block
        snprintf(cmd, 1024, "unsubscribe! block SIDEKIQ-TX:%d\n", d_src_port);
        send_msg( cmd );
        receive_msg( rcv, 1024 ); // clear out rcv buf
#if 0
	// unsubscribe from IQ block
        snprintf(cmd, 1024, "unsubscribe! block IQ:%d\n", d_iq_port);
        send_msg( cmd );
        receive_msg( rcv, 1024 ); // clear out rcv buf
#endif
    }
}

void 
sidekiq_tx::stop()
{
    char cmd[1024];
    char rcv[1024];
    uint8_t count = 0;

    if( DEBUG_SIDEKIQ ) {
	printf("stopping\n");
    }

    if( d_state == STATE_STARTED ) {
	snprintf( cmd, 1024,
		 "config! block SIDEKIQ-TX:%d status disabled\n",
		 d_iq_port );
	send_msg( cmd );
	receive_msg( rcv, 1024 );

#if 0
	// shutdown the IQ socket
	shutdown(d_iq_port, 2);
#endif
    }
    d_state = STATE_STOPPED;
}

void 
sidekiq_tx::send_msg( char * cmd )
{
    int flags = 0;
    if (DEBUG_SIDEKIQ) {
	printf("sending: %s\n", cmd);
    }

    sendto( d_sock, cmd, strlen(cmd)+1, flags,
	    (const sockaddr*)&d_server_addr, d_addr_len);
    // Make sure to not send messages too fast
    usleep(200000);
}

int 
sidekiq_tx::receive_msg( char * rcv, int size )
{
    int flags = 0;
    int num_bytes;

    // TODO: add timeout, fail and check for fail

    memset( rcv, 0, size );
    num_bytes = recvfrom( d_sock, rcv, size, flags,
	                    (struct sockaddr *)&d_server_addr, &d_addr_len);
    if( num_bytes <= 0 )
    {
	throw std::runtime_error("failed to receive response");
    }

    if (DEBUG_SIDEKIQ) {
	printf("receive: %s\n", rcv);
    }

    return num_bytes;
}    
    
void 
sidekiq_tx::config_src()
{
    char cmd[1024];
    char rcv[1024];

    char *pParam;
    char *pValue;
    
    int index=0;

    param_map::iterator iter;

    index = snprintf(cmd, 1024, "config! block SIDEKIQ-TX:%d", d_src_port);
    // configure all of the parameters
    for( iter=sidekiq_params.begin(); 
	 iter != sidekiq_params.end(); 
	 iter++ ) {
	index += snprintf( &cmd[index], 1024-index, " %s ", (iter->first).c_str() );
	// format the parameters based on data_type
	switch( iter->second.data_type ) {
	    case srfs::SRFS_UINT64:
		index += snprintf( &cmd[index], 1024-index, "%lu", 
				   (*(uint64_t*)(iter->second.p_value)) );
		break;

	    case srfs::SRFS_UINT32:
		index += snprintf( &cmd[index], 1024-index, "%u", 
				   (*(uint32_t*)(iter->second.p_value)) );
		break;

	    case srfs::SRFS_UINT16:
		index += snprintf( &cmd[index], 1024-index, "%hu", 
				   (*(uint16_t*)(iter->second.p_value)) );
		break;

	    case srfs::SRFS_UINT8:
		index += snprintf( &cmd[index], 1024-index, "%hhu", 
				   (*(uint8_t*)(iter->second.p_value)) );
		break;

	    case srfs::SRFS_FLOAT:
		index += snprintf( &cmd[index], 1024-index, "%f", 
				   (*(float*)(iter->second.p_value)) );
		break;

	    case srfs::SRFS_ENUM:
		index += snprintf( &cmd[index], 1024-index, "%s", 
				   (iter->second.p_strings[(*(int*)(iter->second.p_value))]).c_str() );
		break;
	}
    }
    index += snprintf( &cmd[index], 1024-index, " status %s\n",
		       status_str[d_srfs_src_status].c_str());
    send_msg( cmd );
    receive_msg( rcv, 1024 );

    // parse the response, update the returned parameters
    pParam = strtok( rcv, " "  );
    while( pParam != NULL )	{
	iter = sidekiq_params.find(pParam);
	if( iter != sidekiq_params.end() ) {
	    pParam = strtok( NULL, " " );
	    update_param( &(iter->second), (const char*)(pParam) );
	}
	else if( strncmp(pParam, "NOK", 4) == 0 ) {
	    throw std::invalid_argument("unexpected error with config_src");
	}
	// get the next parameter
	pParam = strtok( NULL, " " );
    } // end parsing
}

    } // namespace sidekiq
} // namespace gr
