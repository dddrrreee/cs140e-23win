// one way test of sending 4 bytes using N-byted ack'd packets.
//
// why: we use NRF in fixed size mode (you can do dynamic) so can
// easily be that what you want to send is smaller than the packet
// size.   
//
// how: copy the payload into struct of max packet size, padded with
// zeros and send using that.  have to extract the payload at the 
// other end.
// 
#include "nrf-test.h"

// useful to mess around with these. 
enum { ntrial = 1000, timeout_usec = 1000, nbytes = 32 };

// max message size.
typedef struct {
    uint32_t data[NRF_PKT_MAX/4];
} data_t;
_Static_assert(sizeof(data_t) == NRF_PKT_MAX, "invalid size");

// simple method for sending 32 bits: copy into a <data_t> struct,
// send <nbytes> from that.
static inline int send32_ack(nrf_t *nic, uint32_t txaddr, uint32_t x) {
    assert(nbytes>=4);
    data_t d = {0};
    d.data[0] = x;
    return nrf_send_ack(nic, txaddr, &d, nbytes) == nbytes;
}

// simple method for receiing 32 bits: receive into a <data_t> struct,
// copy payload out of that.
static inline int recv32(nrf_t *nic, uint32_t *out) {
    assert(nbytes>=4);
    data_t d;
    int ret = nrf_read_exact_timeout(nic, &d, nbytes, timeout_usec);
    assert(ret<= nbytes);
    if(ret == nbytes)
        memcpy(out, &d, 4);
    return ret;
}

// send 32 byte packets from <server> to <client>.  
static void
one_way_ack(nrf_t *server, nrf_t *client, int verbose_p) {
    unsigned client_addr = client->rxaddr;
    unsigned ntimeout = 0, npackets = 0;

    for(unsigned i = 0; i < ntrial; i++) {
        if(verbose_p && i  && i % 100 == 0)
            trace("sent %d ack'd packets\n", i);

        // output("sent %d\n", i);
        if(!send32_ack(server, client_addr, i))
            panic("send failed\n");

        uint32_t x;
        int ret = recv32(client, &x);
        // output("ret=%d, got %d\n", ret, x);
        if(ret == nbytes) {
            if(x != i)
                nrf_output("client: received %d (expected=%d)\n", x,i);
            assert(x == i);
            npackets++;
        } else {
            if(verbose_p) 
                output("receive failed for packet=%d, nbytes=%d ret=%d\n", i, nbytes, ret);
            ntimeout++;
        }
    }
    trace("trial: total successfully sent %d ack'd packets lost [%d]\n",
        npackets, ntimeout);
    assert((ntimeout + npackets) == ntrial);
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
    one_way_ack(s, c, 1);

    // emit all the stats.
    nrf_stat_print(s, "server: done with one-way test");
    nrf_stat_print(c, "client: done with one-way test");
}
