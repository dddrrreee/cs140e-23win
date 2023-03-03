#ifndef __SPI_H__
#define __SPI_H__

// XXX: need to redo so you can optionally have multiple spi devices active.
// the downside is that i think you will have to reset the cs on each 
// transaction?

// right now we just specialize to the main SPI0 rpi interface; trivial to
// add spi1, spi2.

#define SPI_CE0 1111
#define SPI_CE1 2222

void spi_init(unsigned chip_select, unsigned clock_divider);

// tx and rx must be the same size; easy to have dedicated rx or tx.
int spi_transfer(uint8_t rx[], const uint8_t tx[], unsigned nbytes);

// clk_div must be a power of 2.
// void spi0_init(unsigned clk_div);
// void spi1_init(unsigned clk_div);

// interface that allows use of chip 1 + chip 0
typedef struct {
    unsigned chip;
    unsigned div;

    unsigned mosi,miso,clk,ce;
} spi_t;
int spi_n_transfer(spi_t s, uint8_t rx[], const uint8_t tx[], unsigned nbytes);
spi_t spi_n_init(unsigned chip_select, unsigned clk_div);

void spi_set_verbose(unsigned verbose_p);

static inline spi_t spi_set_mosi(spi_t s, uint8_t mosi) 
    { s.mosi = mosi;  return s; }
static inline spi_t spi_set_miso(spi_t s, uint8_t miso) 
    { s.miso = miso;  return s; }
static inline spi_t spi_set_clk(spi_t s, uint8_t clk) 
    { s.clk = clk;  return s; }
static inline spi_t spi_set_ce(spi_t s, uint8_t ce) 
    { s.ce = ce;  return s; }

static inline spi_t sw_spi_mk(unsigned chip_select, unsigned clk_div) {
#if 0
    assert(chip_select == 0);
    return spi_mk(chip_select, clk_div);
#endif

    assert(chip_select == 0);

    // ce is 20
    enum { 
        sw_miso = 26,
        sw_mosi = 19,
        sw_clk = 13,
        sw_csn = 6
    };

    return (spi_t) { 
            .chip = chip_select, 
            .div = clk_div,
            .mosi = sw_mosi,
            .miso = sw_miso,
            .clk = sw_clk,
            .ce = sw_csn
    };
}

static inline spi_t 
spi_mk(unsigned chip_select, unsigned clk_div) {
    enum {
        // hack: for the moment, leave the same
        mosi     = 10,
        miso     = 9,
        clk      = 11,
        chip_0_ce       = 8,
        chip_1_ce       = 7,
    };
    
    unsigned ce;
    if(chip_select == 0) {
        ce = chip_0_ce;
        return sw_spi_mk(chip_select, clk_div);
    } else if(chip_select == 1)
        ce = chip_1_ce;
    else
        panic("bad chip select: %d\n", chip_select);

    return (spi_t) { 
            .chip = chip_select, 
            .div = clk_div,
            .mosi = mosi,
            .miso = miso,
            .clk = clk,
            .ce = ce
    };
}

// init software spi: user can override pin defaults first.
spi_t sw_spi_init(spi_t s);

static inline void spi_print_config(const char *msg, spi_t s) {
    output("spi config <%s>:\n", msg);
    output("    chip=%d\n", s.chip);
    output("    mosi=%d\n", s.mosi);
    output("    miso=%d\n", s.miso);
    output("    clk=%d\n", s.clk);
    output("    ce=%d\n", s.ce);
} 

#endif 
