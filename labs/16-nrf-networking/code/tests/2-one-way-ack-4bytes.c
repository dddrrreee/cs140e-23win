// one way test of 4-byte ack'd packets.  right now if it gets duplicate
// packets or perm packet loss it will assert out, but collisions/dups do 
// not mean the code is wrong.
//
// you'll notice the bandwidth is awful!   a good extension/project is to 
// improve this and see how close you can get to hw limits.
#include "nrf-test.h"

// useful to mess around with these. 
enum { ntrial = 1000, timeout_usec = 1000 };

// send 4 byte packets from <server> to <client>.  
//
// nice thing about loopback is that it's trivial to know what we are 
// sending, whether it got through, and do flow-control to coordinate
// sender and receiver.
static void
one_way_ack(nrf_t *server, nrf_t *client, int verbose_p) {
    unsigned client_addr = client->rxaddr;
    unsigned ntimeout = 0, npackets = 0;

    for(unsigned i = 0; i < ntrial; i++) {
        if(verbose_p && i  && i % 100 == 0)
            trace("sent %d ack'd packets\n", i);
        nrf_send_ack(server, client_addr, &i, 4);

        // receive from client nic
        uint32_t x;
        if(nrf_read_exact_timeout(client, &x, 4, timeout_usec) == 4) {
            if(x != i)
                nrf_output("client: received %d (expected=%d)\n", x,i);
            assert(x == i);
            npackets++;
        } else {
            if(verbose_p) 
                nrf_output("receive failed for packet=%d, timeout\n", i);
            ntimeout++;
        }
    }
    trace("trial: total successfully sent %d ack'd packets lost [%d]\n",
        npackets, ntimeout);
    assert((ntimeout + npackets) == ntrial);
}

void notmain(void) {
    unsigned nbytes = 4;

    trace("configuring reliable (acked) server=[%x] with %d nbyte msgs\n",
                server_addr, nbytes);

    nrf_t *s = server_mk_ack(server_addr, nbytes);
    nrf_t *c = client_mk_ack(client_addr, nbytes);

    // reset the times so we get a bit better measurement.
    nrf_stat_start(s);
    nrf_stat_start(c);

    // run test.
    one_way_ack(s, c, 1);

    // emit all the stats.
    nrf_stat_print(s, "server: done with one-way test");
    nrf_stat_print(c, "client: done with one-way test");
}
