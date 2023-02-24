We're going to send packets back and forth between your two pi's.
Make sure you carefully go through:
  - the state machine (p22)
  - the different registers (p57-p63) you want to go through each register
    and decide if you need it, and if so, how to set it.
  - the shockburst protocol (p65)

There are some pages and rules listed out below, but they are not 
exhaustive!

Useful pages:
  - p12: power limits: pay attention to max and min.
  - p22: state machine: make sure you know what states to move to and
    how to do it.  we care about RX, TX, standby-I.  stay in the
    "recommended" states.
  - p24: table to modes and how to get in/out.
  - p24: key times that you have to delay when mode switching.  pay
    attention to "power down" -> "standby"!   can be massive.
  - p25: lower the signaling rate, the further the signal goes.
  - p25: higher the signalling rate, the wider the channel.  2mbps
    need jumps of 2+.
  - p27: shockburst: retransmit, acks: given the overhead of switching
    from rx/tx, we use the built in protocol.   we'll just do fixed
    size with acks.   if you want "true" reliability, have to add
    some higher level features.   
  - p28: be careful what address you use!   can interfere with packet
    detection.
  - p33: hardware picture for TX fifo: if a packet is lost, have
    to FLUSH_TX.
  - p34: setting retransmit time, can't be < 500usec at 1mbps or
    2mbps.  defines table.
  - p39: multiceiver --- useful if you have many nodes.  might still
    need to overlay your own network address scheme on top tho.

    i think if you do frequency hopping can do many different overlays
    simultaneously.  the issue is that you don't necessarily know to 
    switch to a channel.  
  - p40: explanation pipe addressing for multiceiver
  - p41: example pipe addressing for multiceiver
  - p50: what SPI / pin signals to send to the rf device.
  - p51: SPI commands defined: TABLE 20 and how to send them.
  - p53: SPI timing ranges.
  - p56: actual rx/tx FIFO and rules.
  - p57-p63: start of register map: you want to go through each register
    and decide if you need it, and if so, how to set it.
  - p65: the enhanced shockburst protocol written out in terms of
    the registers.

Registers from p57-p63--- no guarantee the numbers are correct: double check!:

        CONFIG:         00 (p57)  : configuration
        EN_AA:          01 (p57)  : enable acks, per pipe.
        EN_RXADDR:      02 (p57)  : enable rx addr, per pipe.
        SETUP_AW:       03 (p58)  : Setup address widths (common all pipes).
        SETUP_RETR:     04 (p58)  : Setup auto retran delay.
        RF_CH:          05 (p58)  : set channel for rx/tx.
        RF_SETUP:       06 (p58)  : set data rate, power and other.
        STATUS:         07 (p59)  : rx, tx status register
        OBSERVE_TX      08 (p59)  : count lost and retrans packets.
        RPD             09 (p59)  : we don't use.
        
        RX_ADDR_P0      0A (p59)  : address length for pipe 0.
        RX_ADDR_P1      0B (p60)  : address length for pipe 1.
        RX_ADDR_P2      0C (p60)  : address length for pipe 2.
        RX_ADDR_P3      0D (p60)  : address length for pipe 3.
        RX_ADDR_P4      0E (p60)  : address length for pipe 4.
        RX_ADDR_P5      0F (p60)  : address length for pipe 5.
        
        TX_ADDR         10 (p60)  : address for tx.

        RX_PW_P0        11 (p60)  : number of bytes in rx packet for pipe 0.
        RX_PW_P1        12 (p60)  : number of bytes in rx packet for pipe 1.
        RX_PW_P2        13 (p60)  : number of bytes in rx packet for pipe 2.
        RX_PW_P3        14 (p60)  : number of bytes in rx packet for pipe 3.
        RX_PW_P4        15 (p61)  : number of bytes in rx packet for pipe 4.
        RX_PW_P5        16 (p61)  : number of bytes in rx packet for pipe 5.

        FIFO_STATUS     17 (p61)  : status of rx/tx fifo (full, empty, etc)

        DYNPD           1C (p62)  : enable dynamic packet load.
        FEATURE         1D (p63)  : different features. 


Rules:
  - the node cannot received when in TX mode.  thus, you want to
    minimize the time you are out of RX mode.  our general model: stay
    in RX mode by default, when you have to send, flip to TX, send and
    after ACK, flip back to RX.

  - just b/c the protocol has ACKs does not mean transmit is guaranteed:
    you still have to decide what to do if retransmit attempts are
    exceeded.

  - the rf transciever is an external device with its own power:
    therefore, rebooting the pi has no impact on its state!  i.e.,
    after reboot (rather than a hard power reset) you cannot rely on
    parameters having their "reset" values.

  - similarly: for just b/c you got an ack back *does not mean* the other 
    end received the packet all the way through to the code waiting on it.
    it just means it was stored in the FIFO buffers on the transceiver.
    
    if the code does not pull it off fast enough, it can get overwritten,
    or if the code crashes, it was never acted on.  i don't think this
    matters for today's lab, but in general writing code that falsely
    equates "received ack" with "packet was received and acted on" can
    lead to hard to debug deadlocks of the entire system where one node
    is waiting for an action to happen but the message was never acted
    upon by a higher level.

    this plays out in lots of ways in systems --- it's good to start with
    "the end to end argument" by saltzer, reed, clark.
