// low level routines to talk to NRF over spi + print out configuration.
// <nrf_dump> is useful to look at to figure out how to access the 
// different register values and what they mean
#include "nrf.h"
#include "spi.h"
#include "nrf-hw-support.h"

unsigned nrf_verbose_p = 0;
void nrf_set_verbose(int v_p) {
    nrf_verbose_p = v_p;
}

// initialize spi and the GPIO pins.
spi_t pin_init(unsigned ce, unsigned spi_chip) {
    // p 20 implies takes 100ms to get device online?
    delay_ms(100);
    assert(spi_chip == 0 || spi_chip == 1);

    // the larger you make the clock divider, the slower 
    // SPI goes.
    spi_t s = spi_n_init(spi_chip, 20 /* 1024 */);
    assert(s.chip == spi_chip);

    // give spi time to start-up [not sure if needed]
    delay_ms(100);

    dev_barrier();
    gpio_set_output(ce);
    // alot of transient electrical with these devices, make sure
    // signal is clean.
    gpio_set_pulldown(ce);
    gpio_set_off(ce);
    dev_barrier();

    return s;
}

/************************************************************************
 * low level SPI operations to talk to NRF.
 */

// read the value of nrf <reg>
// as an option, change this to use getn so there is just one copy.
// we did it like this so it's a simpler bit of code to inspect at the 
// very beginning.
uint8_t nrf_get8(const nrf_t *n, uint8_t reg) {
    uint8_t rx[3], tx[3];

    // is this supposed to be three?
    tx[0] = reg;
    tx[1] = NRF_NOP;

    // nrf_debug("sent: tx[0]=%b\n", tx[0]);
    spi_n_transfer(n->spi, rx,tx,2);
    // nrf_debug("recv: rx[0]=%b, rx[1]=%b\n", rx[0], rx[1]);
    return rx[1];
}

// set nrf <reg> = <v>
// as an option, getn so there is just one copy.
// we did it like this so it's a simpler bit of code to inspect at the 
// very beginning. this to use putn
uint8_t nrf_put8(nrf_t *n, uint8_t reg, uint8_t v) {
    uint8_t rx[3], tx[3];

    tx[0] = NRF_WR_REG | reg;
    tx[1] = v;

    // nrf_debug("wr8: sent: tx[0]=%b, tx[1]=%b\n", tx[0], tx[1]);
    spi_n_transfer(n->spi, rx,tx,2);
    // nrf_debug("wr8: recv: rx[0]=%b, rx[1]=%b\n", rx[0], rx[1]);

    return rx[0];   // status.
}

// write out <reg>=<v> and then read it back to make sure things are
// set as expected.
uint8_t nrf_put8_chk_helper(src_loc_t loc, nrf_t *nic, uint8_t reg, uint8_t v) {
    uint8_t status = nrf_put8(nic, reg, v);

    // doing this *always* ran into problems during message sending.
    // so made this seperate routine.
    uint8_t x = nrf_get8(nic, reg);
    if(x != v)
        loc_panic(loc, "reg=<%x>: got <%x>, expected <%x>\n", reg, x, v);
    return status;
}

// send <cmd> and get <nbytes> of data from the nrf.
uint8_t nrf_getn(nrf_t *nic, uint8_t cmd, void *bytes, uint32_t nbytes) {
    uint8_t rx[64], tx[64];
    assert(nbytes < (sizeof tx + 1));
    assert(nbytes > 0);

    // need to fix the spi interface.
    tx[0] = cmd;
    memset(&tx[1], NRF_NOP, nbytes);

    spi_n_transfer(nic->spi, rx,tx,nbytes+1);
    memcpy(bytes, &rx[1], nbytes);

    return rx[0];   // status
}

// write <cmd> with <nbytes> of data to the nrf.
uint8_t nrf_putn(nrf_t *nic, uint8_t cmd, const void *bytes, uint32_t nbytes) {
    uint8_t rx[64], tx[64];
    assert(nbytes < (sizeof tx + 1));
    assert(nbytes > 0);

    // need to fix the spi interface.
    tx[0] = cmd;
    memcpy(&tx[1], bytes, nbytes);
    spi_n_transfer(nic->spi, rx,tx,nbytes+1);

    return rx[0];   // status.
}

uint8_t nrf_or8(nrf_t *n, uint8_t reg, uint8_t v) {
    return nrf_put8(n, reg, nrf_get8(n, reg) | v);
}
// r-m-w: set reg:bit=0
void nrf_bit_clr(nrf_t *n, uint8_t reg, unsigned bit) {
    assert(bit < 8);
    nrf_put8_chk(n, reg, bit_clr(nrf_get8(n, reg), bit));
}
// r-m-w: set reg:bit=1
void nrf_bit_set(nrf_t *n, uint8_t reg, unsigned bit) {
    assert(bit < 8);
    nrf_put8_chk(n, reg, bit_set(nrf_get8(n, reg), bit));
}
// is reg:bit == 1?
int nrf_bit_isset(nrf_t *n, uint8_t reg, uint8_t bit_n) {
    assert(bit_n<8);
    return bit_get(nrf_get8(n, reg), bit_n) == 1;
}

/************************************************************************
 * NRF commands
 */
uint32_t nrf_get_addr(nrf_t *nic, uint8_t reg, unsigned nbytes) {
    assert(nbytes==4 || nbytes==3);
    uint32_t x = 0;
    assert(nbytes == nrf_default_addr_nbytes);
    nrf_getn(nic, reg, &x, nbytes);
    return x;
}

void nrf_set_addr(nrf_t *nic, uint8_t reg, uint32_t addr, unsigned nbytes) {
    assert(nbytes == nrf_default_addr_nbytes);
    assert(nbytes==4 || nbytes==3);

    // works b/c is little endien --- what is the rule for NRF_WR_REG?
    nrf_putn(nic, reg | NRF_WR_REG, &addr, nbytes);

    // read back in and make sure it is out there.
    nrf_opt_assert(nic, nrf_get_addr(nic, reg, nbytes) == addr);
}

// TX_FLUSH,p51: afaik we *only* do this when a reliable 
// tx failed b/c of max attempts (or, probably good:
// on boot up to put the FIFO in a good state).
uint8_t nrf_tx_flush(const nrf_t *n) {
    uint8_t cmd = FLUSH_TX, res = 0;
    spi_n_transfer(n->spi, &res, &cmd, 1);
    return res;     // status.
}

// Flush RX FIFO
uint8_t nrf_rx_flush(const nrf_t *n) {
    uint8_t cmd = FLUSH_RX, res = 0;
    spi_n_transfer(n->spi, &res, &cmd, 1);
    return res;     // status.
}

/*******************************************************************
 * print config: easy way to see how to access registers!
 */

void nrf_conf_print(const char *msg, nrf_conf_t *c) {
    nrf_output("%s: ce_pin=%d, int_pin=%d, spi_chip=%d\n", 
        msg,
        c->ce_pin, 
        c->int_pin, 
        c->spi_chip);
}

void nrf_dump(const char *msg, nrf_t *nic) {
    nrf_output("---------------------------------------------\n");
    nrf_output("%s:\n", msg);
    nrf_conf_print("\tpins", &nic->config);

 

    uint8_t x = nrf_get8(nic, NRF_CONFIG);
    nrf_output("\tCONFIG (%b):\tPWR_UP=%d, PRIM_RX=%d, CRC=%d EN_CRC=%d", 
        x,bit_get(x,1), 
        bit_get(x,0),
        bit_get(x,2),
        bit_get(x,3));
    output(" MASK_RX_DR==%d, MASK_TX_DS=%d, MASK_MAX_RT=%d\n",
            bit_get(x,6), bit_get(x,5), bit_get(x,4));

    x = nrf_get8(nic, NRF_EN_AA);
    nrf_output("\tEN_AA (%b):\t", x);
    if(x == 0b111111)
        output("all pipes have auto-ack enabled\n");
    else {
        output("pipes enabled:");
        for(int i = 0; i < 6; i++)
            if(bit_get(x,i))
                output(" pipe%d=auto-ack ", i);
        output("\n");
    }

    x = nrf_get8(nic, NRF_EN_RXADDR);
    nrf_output("\tEN_RXADDR (%b):", x);
    if(x == 0b111111)
        output("\tall pipes enabled\n");
    else {
        output("\tpipes enabled:");
        for(int i = 0; i < 6; i++)
            if(bit_get(x,i))
                output(" pipe%d=1 ", i);
        output("\n");
    }

    x = nrf_get8(nic, NRF_SETUP_AW);
    assert(x >= 1 && x <= 3);
    unsigned addr_nbytes = nrf_get_addr_nbytes(nic);
    assert(nrf_get_addr_nbytes(nic) == x + 2);

    nrf_output("\tSETUP_AW:\taddress=%d bytes\n", addr_nbytes);

    x = nrf_get8(nic, NRF_SETUP_RETR);
    unsigned delay = ((x >> 4)+1) * 250;
    unsigned retry = x & 0b1111;
    nrf_output("\tSETUP_RETR (=%b):\tretran delay=%dusec, attempts=%d\n", x, delay, retry);

    x = nrf_get8(nic, NRF_RF_CH);
    nrf_output("\tRF_CH(=%b):\t2.%dMhz\n", x, 400 + x); // i think this is correct
    
    x = nrf_get8(nic, NRF_RF_SETUP);
    assert(!bit_get(x,5));
    unsigned bwidth = bit_get(x,3) ? 2 : 1;
    int pwr;
    switch(bits_get(x,1,2)) {
    case 0b00: pwr = -18; break;
    case 0b01: pwr = -12; break;
    case 0b10: pwr = -6; break;
    case 0b11: pwr = 0; break;
    default: panic("bad encoding\n");
    }
    nrf_output("\tRF_SETUP(=%b):\tbwidth = %dMbs, power= %ddBm\n", x, bwidth, pwr);

    x = nrf_get8(nic, NRF_STATUS);
    nrf_output("\tSTATUS(=%b): RX-int=%d, TX-int=%d, RT-int=%d RXpipe=%b (%s), TX-full=%d\n",
            x, 
            bit_get(x, 6),
            bit_get(x, 5),
            bit_get(x, 4),
            bits_get(x, 1, 3),
            (bits_get(x, 1, 3) == 0b111) ? "empty" : "has data!",
            
            bit_get(x, 0));
    
    
    uint64_t txaddr = 0;
    nrf_getn(nic, NRF_TX_ADDR, &txaddr, addr_nbytes);
    nrf_output("\tTX_ADDR = %llx\n", txaddr);

    unsigned pipes_en = nrf_get8(nic, NRF_EN_RXADDR);
    for(unsigned i = 0; i < 6; i++) {
        if(!bit_get(pipes_en, i))
            continue;
        uint64_t addr = 0;

        nrf_getn(nic, NRF_RX_ADDR_P0+i, &addr, addr_nbytes);
        nrf_output("\tRX_ADDR_P%d = %llx\n", i, addr);

        x = nrf_get8(nic, NRF_RX_PW_P0+i);
        nrf_output("\tRX_PW_P%d (%b): %d byte fixed size packets\n", i, x, x);
    }
    
    x = nrf_get8(nic, NRF_FIFO_STATUS);
    nrf_output("\tFIFO_STATUS(=%b): TX_FULL=%d, TX_EMPTY=%d, RX_FULL=%d, RX_EMPTY=%d\n",
        x,
        bit_get(x,5),
        bit_get(x,4),
        bit_get(x,1),
        bit_get(x,0));

    assert(nrf_get8(nic, NRF_FEATURE) == 0);
    assert(nrf_get8(nic, NRF_DYNPD) == 0);
    nrf_output("---------------------------------------------\n");
}


/********************************************************************
 * print out stats.
 */

// idk if it makes sense to put this elsewhere?
typedef struct {
    unsigned whole;
    unsigned fraction;
} frac_t;

static inline frac_t
kbytes_per_sec(unsigned tot_bytes, unsigned tot_usec) {
    unsigned tot_ms = tot_usec / 1000;

    // bytes per sec
    uint32_t b = (1000*tot_bytes)  / tot_ms;

    return (frac_t) { .whole = b / 1000, .fraction = b % 1000 };
}

#include "pretty-time.h"
void nrf_stat_print(nrf_t *nic, const char *fmt, ...) {
    uint32_t start = nic->start_usec;
    uint32_t tot_usec = timer_get_usec() - start;

    output("NRF:");
    va_list args;
    va_start(args, fmt);
       vprintk(fmt, args);
    va_end(args);
    output("\n");
    nrf_output("\ttotal time= [");
    pretty_print_usec(start);
    output("]\n");

    
    uint32_t nmsgs = nic->tot_sent_msgs;
    if(nmsgs) 
        nrf_output("\ttotal sent messages=%d\n", nmsgs);
    uint32_t tot_bytes = nic->tot_sent_bytes;
    if(tot_bytes) {
        frac_t bw = kbytes_per_sec(tot_bytes, tot_usec);
        nrf_output("\ttotal sent bytes=%d bandwidth=[%d.%d kb/s]\n", 
            tot_bytes, bw.whole, bw.fraction);
    }
    // these are not deterministic.
    if(nic->tot_retrans)
        output("\ttotal retrans=%d\n", nic->tot_retrans);
    if(nic->tot_lost)
        output("\ttotal lost=%d\n", nic->tot_lost);

    nmsgs = nic->tot_recv_msgs;
    if(nmsgs)
        nrf_output("\ttotal received messages=%d\n", nmsgs);
    tot_bytes = nic->tot_recv_bytes;
    if(tot_bytes) {
        frac_t bw = kbytes_per_sec(tot_bytes, tot_usec);
        nrf_output("\ttotal received bytes=%d bandwidth=[%d.%d kb/s]\n", 
                tot_bytes, bw.whole, bw.fraction);
    }
}
