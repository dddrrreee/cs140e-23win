#ifndef __NRF_H__
#define __NRF_H__
// NRF interface.  oversimplified for lab setting.  defines:
//   - <nrf_t> NRF structure
//   - <nrf_conf_t> configuration structure.
//   - the assert and debugging routines.
//   - the main prototypes.
//
// NOTE: <nrf-hw-support.h> has the register enums and helper routines
// for manipulating.
//
// don't modify for lab, but you should rewrite to make it yours if 
// you want to use for real.
//  1. we have a single pipe [nrf supports 6]
//  2. should have a connection structure that bundles the receipient's
//     address, packet size, etc.
//  3. should have a nrf_rx_t structure that lets you have different 
//     pipes, with a queue, etc.
//  4. probably need to do interrupts in a real system.
//
//  we could easily have four significant labs (or a whole class, or a whole
//  career) on networking and making distributed systems that use these
//  devices.  two useful, easily understood extensions:
//  -  make a simple reliable FIFO protocol. can be much simpler than 
//     TCP since the hardware will handle a bunch of the issues "mostly".   
//     retrans should be more of an exceptional situation, so you can 
//     be more simple minded about it.  but, there's more subtly than 
//     it might seem.  [e.g., if your node reboots, and a message with 
//     an old connection id arrives after, what happens?]
//
//   - make a simple network boot implementation on top of this.

#include "rpi.h"
#include "spi.h"        // hw spi implementation.
#include "circular.h"   // for the circular queue.

// maximum hw supported packet size.
enum { NRF_PKT_MAX = 32 };

// configuration settigs for NRF.
typedef struct {
    uint8_t spi_chip,   // which spi chip are we using.
                        // this isn't the right setup since makes
                        // sw spi a bit weird.
            

            ce_pin,     // GPIO pin used to signal NRF.
            int_pin;    // GPIO pin used for interrupts.  we don't use today.
    uint8_t nbytes;     // how many bytes the the packet is.
    uint8_t channel;    // what MHz channel the RF is using.
} nrf_conf_t;

typedef struct nrf {
    // values for the many different hardware settings.
    nrf_conf_t config;
    // the spi configuration 
    spi_t spi;

    // our receive address
    // for lab, we only use 1 pipe, but the hardware provides 6 possibile.
    // if you extend will have to extend the receive routines to specify
    // the pipe you want.  
    // note:
    //  - as with UART, the hardware has limited buffering.  if you don't
    //    read a message soon enough, and others arrive, the hardware
    //    will drop stuff.  unless you are disciplined with keeping
    //    the code path between checking the hw (polling) short, then
    //    it's interrupts. 
    uint32_t rxaddr;

    // queue holding message data in order of arrival.  note:
    //   - if you have hardware acks, there can be duplicates (caused 
    //     by retrans). 
    //   - if you have multiple senders (or even one) and are waiting for
    //     a specific message, others messages can obviously arrive so 
    //     so you need to have some strategy in place to defer and handle
    //     them later, or if want to handle now, what to do.
    cq_t     recvq;

    void     *data; // opaque client data pointer.

    // some simple statistics.  maybe seperate this out into a struct.
    uint32_t start_usec,
             tot_sent_msgs,
             tot_sent_bytes,

             tot_retrans,
             tot_lost,

             tot_recv_msgs,
             tot_recv_bytes;
} nrf_t;

// initialize the NRF hardware described by config <c>  (which specifies
// the SPI and GPIO pins used) to have the receive address <rxaddr>.
// <ack_p> indicates whether it's acked or not.
//
// useful extension is to have multiple pipes so at least one can be
// unacked (for broadcast updates from a server) and some acked.
nrf_t * nrf_init(const nrf_conf_t c, uint32_t rxaddr, unsigned acked_p);
nrf_t * staff_nrf_init(const nrf_conf_t c, uint32_t rxaddr, unsigned acked_p);

static inline nrf_t *nrf_init_acked(nrf_conf_t c, uint32_t rxaddr) {
    return nrf_init(c,rxaddr,1);
}
static inline nrf_t *nrf_init_noack(nrf_conf_t c, uint32_t rxaddr) {
    return nrf_init(c,rxaddr,0);
}


// call to pull packets off of NRF hardware and stick into the receive
// queue <recvq>.
//
// as noted above: if you don't call frequently enough, packets will be
// discarded.
int nrf_get_pkts(nrf_t *n);
int staff_nrf_get_pkts(nrf_t *n);

// send <nbytes> of data pointed to by <msg> to <txaddr> using NRF <n>,
// expect a hardware ack.
int nrf_tx_send_ack(nrf_t *n, uint32_t txaddr, const void *msg, unsigned nbytes);
int staff_nrf_tx_send_ack(nrf_t *n, uint32_t txaddr, const void *msg, unsigned nbytes);
// send <nbytes> of data pointed to by <msg> to <txaddr> using NRF <n>,
// no hardware ack.
int nrf_tx_send_noack(nrf_t *n, uint32_t txaddr, const void *msg, unsigned nbytes);
int staff_nrf_tx_send_noack(nrf_t *n, uint32_t txaddr, const void *msg, unsigned nbytes);


// print out the NRF configuration by reading it from the hardware.
// we give you this.  you should use it alot during lab to make sure 
// that things are looking like what you expect.
void nrf_dump(const char *msg, nrf_t *n);
void nrf_conf_print(const char *msg, nrf_conf_t *c);

// trivial stat stuff.
static inline void nrf_stat_start(nrf_t *nic) {
    nic->start_usec = timer_get_usec();
    nic->tot_sent_msgs = 0;
    nic->tot_sent_bytes = 0;
    nic->tot_retrans = 0;
    nic->tot_lost = 0;
    nic->tot_recv_msgs = 0;
    nic->tot_recv_bytes = 0;
}
void nrf_stat_print(nrf_t *nic, const char *msg, ...);

/*********************************************************************
 * nrf public methods on top of the NRF driver.  
 *   - they are all in nrf-public.c
 *
 * you shouldn't have to modify these implementations.  probably 
 * should be in a different header, but we keep it here b/c lab 
 * is time-limited.
 */

// how many bytes are in the <recv> queue.  will try to pull from 
// NRF.
int nrf_nbytes_avail(nrf_t *n);

// non-blocking read exactly <nbytes> of data into <msg> 
// from NRF <nic>.  does not do short-reads.  if there isn't
// enough data, returns.
//
// returns:
//  - <nbytes> on successful read.
//  - 0 if there was < <nbytes> of data.
int nrf_read_exact_noblk(nrf_t *nic, void *msg, unsigned nbytes);

// blocking read of exactly <nbytes> of data from <nic> into <msg>
// keeps trying until it gets <nbytes>
int nrf_read_exact(nrf_t *nic, void *msg, unsigned nbytes);

// read of exactly <nbytes> of data from <nic> into <msg>
// keeps trying until it gets <nbytes> or roughly <usec_timeout>
// microseconds passes.
// returns:
//  - <-1> if timeout.
//  - <nbytes> if success.
int nrf_read_exact_timeout(nrf_t *nic, void *msg, unsigned nbytes,
    unsigned usec_timeout);

// send an acked packet <msg> of <nbytes> to address <txaddr>
// using NRF <nic>
int nrf_send_ack(nrf_t *nic, uint32_t txaddr, const void *msg, unsigned nbytes);

// send a non-acked packet <msg> of <nbytes> to address <txaddr>
// using NRF <nic>
int nrf_send_noack(nrf_t *nic, uint32_t txaddr, const void *msg, unsigned nbytes);

/********************************************************************
 * configuration support.
 *
 * this should probably be seperated out, but we put it here so make
 * a bit easier to navigate in lab.
 */

#define RF_DR_HI(x) ((x) << 3)
#define RF_DR_LO(x) ((x) << 5)
// clear both bits.
#define RF_DR_CLR(x) ((x) & ~(RF_DR_HI(1) | RF_DR_LO(1)))
typedef enum {
    nrf_1Mbps   = 0,            // both RF_DR_HI=0 and RF_DR_LO=0.
    nrf_2Mbps   = RF_DR_HI(1),  // RF_DR_LO=0, RF_DR_HI=1.
    nrf_250kbps = RF_DR_LO(1),  // RF_DR_LO=1, RF_DR_HI=0.
} nrf_datarate_t;


typedef enum {
    dBm_minus18 = 0b00 << 1, // 7mA
    dBm_minus12 = 0b01 << 1, // 7.5mA
    dBm_minus6  = 0b10 << 1, // 9mA
    dBm_0       = 0b11 << 1, // 11mA
} nrf_db_t;

// these you can modify.
#include "nrf-default-values.h"

static inline int nrf_legal_Mhz(unsigned d) {
    return d < (1<<8);
}
static inline int nrf_legal_nbytes(unsigned nbytes) {
    return (nbytes > 0 && nbytes <= 32);
}
static inline unsigned 
nrf_get_addr_nbytes(nrf_t *nic) {
    return nrf_default_addr_nbytes;
}

static inline nrf_conf_t
nrf_conf_mk(uint32_t nbytes, uint32_t ch) {
    assert(nrf_legal_Mhz(ch));
    assert(nrf_legal_nbytes(nbytes));

    return (nrf_conf_t) { 
        .nbytes = nbytes, 
        .channel = ch, 
    };
}

/***********************************************************************
 * debug support: 
 *  - main thing are asserts that (1) take an NRF structure for error
 *    printing that (2) can be easily disabled for speed.
 */

// prepend NRF to output.
#define nrf_output(args...) do { output("NRF:"); output(args); } while(0)

// if you want to completely elminate <nrf_assert> and 
// <nrf_debug> for speed:
//#define NRF_NDEBUG
#ifdef NRF_NDEBUG
#   define nrf_assert(_nic, bool)   do { } while(0)
#   define nrf_debug(args...)       do { } while(0)
#else
    // debug statement that you can turn off and on.
    extern unsigned nrf_verbose_p;
#   define nrf_debug(args...) \
        do { if(nrf_verbose_p) output(args); } while(0)

    // assert with nic.
#   define nrf_assert(_nic, bool) do {                      \
        if((bool) == 0) {                                   \
            nrf_dump("dying with a panic\n", _nic);         \
            panic("%s", #bool);                             \
        }                                                   \
    } while(0)
#endif

// these are "optional" asserts on the cricital path: make 
// it easy to disable while keeping the rest of the crucial asserts.
//
// using the enum <opt_check_p> should be identical to the 
// <NRF_NDEBUG> define above, but is viewed as somewhat cleaner
// since the code still gets syntactically checked and then 
// removed by the optimizer rather than just completely removed
// before the compiler can vet it.
enum { opt_check_p = 1 };

// assert on fast paths that can be removed for speed.
#define nrf_opt_assert(nic, cond) do {  \
    if(opt_check_p)                     \
        nrf_assert(nic, cond);          \
} while(0)

// put8_chk on fast paths that can be removed for speed.
#define opt_put8_chk(_n,  _reg, _v) do {    \
    if(opt_check_p)                         \
        nrf_put8_chk(_n, _reg, _v);         \
    else                                    \
        nrf_put8(_n, _reg, _v);             \
} while(0)

#endif
