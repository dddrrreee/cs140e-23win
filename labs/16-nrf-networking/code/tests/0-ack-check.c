// configure and dump ack'd hardware configuration.  get these working first.
#include "nrf-test.h"

void notmain(void) {
    unsigned nbytes = 32;

    trace("configuring reliable (acked) server=[%x] with %d nbyte msgs\n", 
                server_addr, nbytes);
    // nrf-test.h
    nrf_t *server_nic = server_mk_ack(server_addr, nbytes);
    nrf_dump("reliable server config:\n", server_nic);


    trace("configuring reliable (acked) client=[%x] with %d nbyte msg\n", 
                client_addr, nbytes);
    // nrf-test.h
    nrf_t *client_nic = client_mk_ack(client_addr, nbytes);
    nrf_dump("reliable client config:\n", client_nic);
}
