// ping pong 4-byte packets back and forth.
#include "nrf-test.h"

// useful to mess around with these. 
enum { ntrial = 1000, timeout_usec = 1000, nbytes = 4 };

// example possible wrapper to recv a 32-bit value.
static int net_get32(nrf_t *nic, uint32_t *out) {
    int ret = nrf_read_exact_timeout(nic, out, 4, timeout_usec);
    if(ret != 4) {
        debug("receive failed: ret=%d\n", ret);
        return 0;
    }
    return 1;
}
// example possible wrapper to send a 32-bit value.
static void net_put32(nrf_t *nic, uint32_t txaddr, uint32_t x) {
    int ret = nrf_send_ack(nic, txaddr, &x, 4);
    if(ret != 4)
        panic("ret=%d, expected 4\n");
}

// send 4 byte packets from <server> to <client>.  
//
// nice thing about loopback is that it's trivial to know what we are 
// sending, whether it got through, and do flow-control to coordinate
// sender and receiver.
static void
ping_pong_ack(nrf_t *s, nrf_t *c, int verbose_p) {
    unsigned client_addr = c->rxaddr;
    unsigned server_addr = s->rxaddr;
    unsigned npackets = 0, ntimeout = 0;
    uint32_t exp = 0, got = 0;

    for(unsigned i = 0; i < ntrial; i++) {
        if(verbose_p && i  && i % 100 == 0)
            trace("sent %d ack'd packets [timeouts=%d]\n", 
                    npackets, ntimeout);

        net_put32(s, client_addr, ++exp);
        if(!net_get32(c, &got))
            ntimeout++;
        // could be a duplicate
        else if(got != exp)
            nrf_output("client: received %d (expected=%d)\n", got,exp);
        else
            npackets++;

        net_put32(c, server_addr, ++exp);
        if(!net_get32(s, &got))
            ntimeout++;
        else if(got != exp)
            nrf_output("server: received %d (expected=%d)\n", got,exp);
        else
            npackets++;
    }
    trace("trial: total successfully sent %d ack'd packets lost [%d]\n",
        npackets, ntimeout);
}

void notmain(void) {
    // configure server
    trace("send total=%d, %d-byte messages from server=[%x] to client=[%x]\n",
                ntrial, nbytes, server_addr, client_addr);

    nrf_t *s = server_mk_ack(server_addr, nbytes);
    nrf_t *c = client_mk_ack(client_addr, nbytes);

    nrf_stat_start(s);
    nrf_stat_start(c);

    // run test.
    ping_pong_ack(s, c, 1);

    // emit all the stats.
    nrf_stat_print(s, "server: done with one-way test");
    nrf_stat_print(c, "client: done with one-way test");
}
