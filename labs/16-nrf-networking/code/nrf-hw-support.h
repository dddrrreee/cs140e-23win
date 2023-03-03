#ifndef __NRF_REGISTERS_H__
#define __NRF_REGISTERS_H__
#include "rpi.h"
#include "libc/bit-support.h"
#include "nrf.h"
#include "src-loc.h"

// initialize spi and the GPIO pins.
spi_t pin_init(unsigned ce, unsigned spi_chip);

// simple utility routines and shared enums.

// register numbers.  p57
enum {
    NRF_CONFIG      = 0x0,
    NRF_EN_AA       = 0x1,
    NRF_EN_RXADDR   = 0x2,
    NRF_SETUP_AW    = 0x3,
    NRF_SETUP_RETR  = 0x4,
    NRF_RF_CH       = 0x5,
    NRF_RF_SETUP    = 0x6,

    NRF_STATUS      = 0x7,
    NRF_OBSERVE_TX  = 0x8,
    NRF_RPD         = 0x9,

    NRF_RX_ADDR_P0  = 0xa,
    NRF_RX_ADDR_P1  = 0xb,
    NRF_RX_ADDR_P2  = 0xc,
    NRF_RX_ADDR_P3  = 0xd,
    NRF_RX_ADDR_P4  = 0xe,
    NRF_RX_ADDR_P5  = 0xf,

    NRF_TX_ADDR     = 0x10,

    NRF_RX_PW_P0    = 0x11,
    NRF_RX_PW_P1    = 0x12,
    NRF_RX_PW_P2    = 0x13,
    NRF_RX_PW_P3    = 0x14,
    NRF_RX_PW_P4    = 0x15,
    NRF_RX_PW_P5    = 0x16,

    NRF_FIFO_STATUS = 0x17,

    NRF_DYNPD       = 0x1c,
    NRF_FEATURE     = 0x1d,

};


// cmds: p 51
enum { 
    NRF_WR_REG              = 0b00100000,
    NRF_R_RX_PAYLOAD        = 0b01100001,
    NRF_W_TX_PAYLOAD        = 0b10100000,
    W_TX_PAYLOAD_NO_ACK     = 0b10110000,
    FLUSH_TX                = 0b11100001,
    FLUSH_RX                = 0b11100010,
    NRF_NOP                 = 0b11111111,
};

// these are simple utility routines to SPI read/write registers
// on the NRF24L01+.

// read 8 bits of data from <reg>
uint8_t nrf_get8(const nrf_t *n, uint8_t reg);
// write 8 bits of data to <reg>
uint8_t nrf_put8(nrf_t *nic, uint8_t reg, uint8_t v);
// write 8-bit <v> to <reg> and then check make get8(reg) = <v>.
uint8_t nrf_put8_chk_helper(src_loc_t l, nrf_t *nic, uint8_t reg, uint8_t v);

#define nrf_put8_chk(nic, reg, v) \
    nrf_put8_chk_helper(SRC_LOC_MK(), nic, reg, v)

// read <nbytes> of data pointed to by <bytes> from <reg>
uint8_t nrf_getn(nrf_t *nic, uint8_t reg, void *bytes, uint32_t nbytes);
// write <nbytes> of data pointed to by <bytes> to <reg>
uint8_t nrf_putn(nrf_t *nic, uint8_t reg, const void *bytes, uint32_t nbytes);

// reg = reg | v
uint8_t nrf_or8(nrf_t *n, uint8_t reg, uint8_t v);

// rmw: set reg:bit=0
void nrf_bit_clr(nrf_t *n, uint8_t reg, unsigned bit);
// rmw: set reg:bit=1
void nrf_bit_set(nrf_t *n, uint8_t reg, unsigned bit);
// is reg:bit == 1?
int nrf_bit_isset(nrf_t *n, uint8_t reg, uint8_t bit_n);


uint32_t nrf_get_addr(nrf_t *nic, uint8_t reg, unsigned nbytes);
void nrf_set_addr(nrf_t *nic, uint8_t reg, uint32_t addr, unsigned nbytes);

uint8_t nrf_tx_flush(const nrf_t *n);
uint8_t nrf_rx_flush(const nrf_t *n);

/********************************************************************
 * hardware accessors.  XXX: go through and sort by register 
 * number and add page numbers.
 */

//* CONFIG=0x0, p57
static inline int nrf_is_pwrup(nrf_t *nic) {
#   define PWR_UP   1
    return bit_isset(nrf_get8(nic, NRF_CONFIG), PWR_UP);
}
static inline void nrf_set_pwrup_off(nrf_t *nic) {
    nrf_bit_clr(nic, NRF_CONFIG, PWR_UP);
    assert(!nrf_is_pwrup(nic));
}
static inline void nrf_set_pwrup_on(nrf_t *nic) {
    nrf_bit_set(nic, NRF_CONFIG, PWR_UP);
    assert(nrf_is_pwrup(nic));
}

static inline int nrf_is_rx(nrf_t *nic) {
#   define PRIM_RX  0
    return bit_isset(nrf_get8(nic, NRF_CONFIG), PRIM_RX);
}
static inline int nrf_is_tx(nrf_t *nic) {
    return nrf_is_rx(nic) == 0;
}

//* EN_AA=0x1, p57
static inline int nrf_pipe_is_acked(nrf_t *n, unsigned p) {
    return bit_get(nrf_get8(n, NRF_EN_AA), p) == 1;
}

//* EN_RXADDR=0x2, p57
static inline int nrf_pipe_is_enabled(nrf_t *n, unsigned p) {
    return bit_get(nrf_get8(n, NRF_EN_RXADDR), p) == 1;
}

//* STATUS=0x7, p59
static inline int pipeid_empty(unsigned pipeid) {
    return pipeid == 0b111;
}
static inline int pipeid_get(uint8_t status) {
    return bits_get(status, 1, 3);
}
static inline int nrf_rx_get_pipeid(nrf_t *n) {
#   define NRF_PIPEID_EMPTY 0b111 // p 59
    return pipeid_get(nrf_get8(n, NRF_STATUS));
}

// clear interrupt = write a 1 in the right position.
static inline int nrf_has_max_rt_intr(nrf_t *nic) {
#   define MAX_RT 4
    return nrf_bit_isset(nic, NRF_STATUS, MAX_RT);
}
static inline void nrf_rt_intr_clr(nrf_t *nic) {
    nrf_put8(nic, NRF_STATUS, 1 << MAX_RT);
}

static inline int nrf_has_tx_intr(nrf_t *nic) {
#   define TX_DS 5
    return nrf_bit_isset(nic, NRF_STATUS, TX_DS);
}
static inline void nrf_tx_intr_clr(nrf_t *nic) {
    nrf_put8(nic, NRF_STATUS, 1 << TX_DS);
}

static inline int nrf_has_rx_intr(nrf_t *nic) {
#   define RX_DR 6
    return nrf_bit_isset(nic, NRF_STATUS, RX_DR);
}
static inline void nrf_rx_intr_clr(nrf_t *nic) {
    nrf_put8(nic, NRF_STATUS, 1 << RX_DR);
}

//* FIFO_STATUS=0x17, p61

// is rx queue empty?
static inline int nrf_rx_fifo_empty(nrf_t *nic) {
    return nrf_bit_isset(nic, NRF_FIFO_STATUS, 0);
}
static inline int nrf_rx_fifo_full(nrf_t *nic) {
    return nrf_bit_isset(nic, NRF_FIFO_STATUS, 1);
}
static inline int nrf_tx_fifo_empty(nrf_t *nic) {
    return nrf_bit_isset(nic, NRF_FIFO_STATUS, 4);
}
static inline int nrf_tx_fifo_full(nrf_t *nic) {
    return nrf_bit_isset(nic, NRF_FIFO_STATUS, 5);
}

static inline int nrf_ce_val(nrf_t *nic) {
    return gpio_read(nic->config.ce_pin);
}

// p 22: CE=0 and PWRUP=1
static inline int nrf_is_standbyI(nrf_t *nic) {
    return nrf_is_pwrup(nic) && nrf_ce_val(nic) == 0;
}

static int nrf_rx_has_packet(nrf_t *nic) {
    return !nrf_rx_fifo_empty(nic);
}

#endif
