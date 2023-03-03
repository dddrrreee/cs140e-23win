#ifndef __NRF_DEFAULT_VALUES_H__
#define __NRF_DEFAULT_VALUES_H__

// seperated out so you can mess with to control values.
// if you are getting conflicts, you have two options:
//   1. change the client/server addresses.   easiest!
//   2. change the frequency.   this seems very sensitive.

// default client and server.
// addresses can increase error.  these have worked:
//  0xe5e5e5,
//  0xe3e3e3,
//  0xe7e7e7,
//  0xe1e1e1,
//  0xd3d3d3,

enum {
    server_addr = 0xd5d5d5,
    client_addr = 0xe5e5e5,
};

enum {
    nrf_default_nbytes              = 4,            // 4 byte packets.

    // allegedly semi-safe from interference
    // RF is really sensitive.   maybe worth writing the code to find.
    // bounce around, send/recv and check.
    nrf_default_channel             = 113,          

    // lower data rate ==> longer distance.
    nrf_default_data_rate           = nrf_2Mbps,    

    // this is full power.
    nrf_default_db                  = dBm_0,

    // 6 retran attempts
    nrf_default_retran_attempts     = 6,            

    // 2000 usec retran delay
    nrf_default_retran_delay        = 2000,         

    // if we increase?  doesn't seem to matter; 4,5 also legal.
    nrf_default_addr_nbytes         = 3,
};

#endif
