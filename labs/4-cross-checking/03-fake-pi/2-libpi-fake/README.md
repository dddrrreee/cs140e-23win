For this libpi: we pull in the actual libpi source for:
  -  printk
  -  putk 
  - putchar 
  - clean_reboot.

the lets us test the actual code.


we just have to define:

   - uart_putc: this is a uart routine that writes a single byte.
   - uart_flush_tx: flushes the uart TX FIFO.

        void uart_flush_tx(void) {
            fflush(stdout);
        }

   - delay_ms: we don't actually delay:

        void delay_ms(unsigned ms) {
        }

   - rpi_reboot:

        void rpi_reboot(void) { 
            exit(0);
        }
