// configure and dump hardware configuration.  get these working first.
#include "nrf-test.h"

void notmain(void) {
    unsigned nbytes = 32;

    trace("configuring no-ack server=[%x] with %d nbyte msgs\n", 
                server_addr, nbytes);
    // nrf-test.h
    nrf_t *server_nic = server_mk_noack(server_addr, nbytes);
    nrf_dump("unreliable server config:\n", server_nic);

    trace("configuring no-ack client=[%x] with %d nbyte msg\n", 
                client_addr, nbytes);
    // nrf-test.h
    nrf_t *client_nic = client_mk_noack(client_addr, nbytes);
    nrf_dump("unreliable client config:\n", client_nic);
}
