// public interface for the nrf driver.
#include "nrf.h"
#include "timeout.h"

// returns the number of bytes available.
// pulls data off if it's on the interface (since there can't be that much
// and we'd start dropping stuff).
int nrf_nbytes_avail(nrf_t *n) {
    while(nrf_get_pkts(n))
        ;
    return cq_nelem(&n->recvq);
}

// non-blocking: if there is less than <nbytes> of data, return 0 immediately.
//    otherwise read <nbytes> of data into <msg> and return <nbytes>.
int nrf_read_exact_noblk(nrf_t *nic, void *msg, unsigned nbytes) {
    assert(nbytes > 0);

    // explicitly, obviously return 0 if n == 0.
    unsigned n;
    if(!(n = nrf_nbytes_avail(nic)))
        return 0;
    if(n < nbytes)
        return 0;

    // fix this stupid circular queue with all the extra volatiles.
    cq_pop_n(&nic->recvq, msg, nbytes);
    return nbytes;
}

int nrf_read_exact_timeout(nrf_t *nic, void *msg, unsigned nbytes, 
    unsigned usec_timeout) {
    timeout_t t = timeout_start();
    int n;
    while(!(n = nrf_read_exact_noblk(nic, msg, nbytes))) {
        // put a yield in here while busywaiting: if there are threads
        // would work.

        // if we are running a thread, would switch

        if(timeout_usec(&t, usec_timeout))
            return -1;
        rpi_wait();
    }
    return n;
}

enum { NRF_TIMEOUT = 10};

// blocking: read exactly <nbytes> of data.
// we have it as an int in case we need to return < 0 errors.
int nrf_read_exact(nrf_t *nic, void *msg, unsigned nbytes) {
    while(1) {
        unsigned usec = NRF_TIMEOUT * 1000 * 1000;
        int n = nrf_read_exact_timeout(nic, msg, nbytes, usec);
        if(n == nbytes) 
            return n;

        if(n < 0) {
            debug("addr=%x: connection error: no traffic after %d seconds\n",
                    nic->rxaddr,  NRF_TIMEOUT);
            // nrf_dump("timeout config\n");
            panic("fix nrf_dump\n");
        }
        assert(n< nbytes);
    }
}

// we should have a tx structure that gives the size of the 
// actual receipient.

// for our tests, the sender and the receiver are setup the same,
// but in general you won't be able to know from the sender if
// <txaddr>:
//   - is setup for ack or no-ack
//   - the message size.
// in a full system we'd have a connection that contains all of this
// but for a short, lab, we just expose things (can build on top
// as an extension!).
int nrf_send_ack(nrf_t *nic, uint32_t txaddr, 
                const void *msg, unsigned nbytes) {
    return nrf_tx_send_ack(nic, txaddr, msg, nbytes);
}
int nrf_send_noack(nrf_t *nic, uint32_t txaddr, 
                const void *msg, unsigned nbytes) {
    return nrf_tx_send_noack(nic, txaddr, msg, nbytes);
}

