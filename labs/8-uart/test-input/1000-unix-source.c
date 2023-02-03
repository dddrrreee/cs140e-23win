0000: // thompson's complete original unix code.  it's an astonishing
0000: // monument to concise power.
0000: //
0000: // copied from:
0000: //    from lion's commentary: https://warsus.github.io/lions-/
0000: //
0000: // consider buying lion's commentary (recommended): 
0000: //
0000: // https://www.amazon.com/Lions-Commentary-Unix-John/dp/1573980137
0000: //
0000: ******************************************************************
0000:
0100: /* fundamental constants: cannot be changed */
0101: 
0102: 
0103: #define USIZE   16              /* size of user block (*64) */
0104: #define NULL    0
0105: #define NODEV   (-1)
0106: #define ROOTINO 1               /* i number of all roots */
0107: #define DIRSIZ  14              /* max characters per directory */
0108: 
0109: 
0110: /* signals: dont change */
0111: 
0112: 
0113: #define NSIG    20
0114: #define         SIGHUP  1       /* hangup */
0115: #define         SIGINT  2       /* interrupt (rubout) */
0116: #define         SIGQIT  3       /* quit (FS) */
0117: #define         SIGINS  4       /* illegal instruction */
0118: #define         SIGTRC  5       /* trace or breakpoint */
0119: #define         SIGIOT  6       /* iot */
0120: #define         SIGEMT  7       /* emt */
0121: #define         SIGFPT  8       /* floating exception */
0122: #define         SIGKIL  9       /* kill */
0123: #define         SIGBUS  10      /* bus error */
0124: #define         SIGSEG  11      /* segmentation violation */
0125: #define         SIGSYS  12      /* sys */
0126: #define         SIGPIPE 13      /* end of pipe */
0127: 
0128: /* tunable variables */
0129: 
0130: #define NBUF    15              /* size of buffer cache */
0131: #define NINODE  100             /* number of in core inodes */
0132: #define NFILE   100             /* number of in core file structures */
0133: #define NMOUNT  5               /* number of mountable file systems */
0134: #define NEXEC   3               /* number of simultaneous exec's */
0135: #define MAXMEM  (64*32)         /* max core per process 
0136:                                                 - first # is Kw */
0137: #define SSIZE   20              /* initial stack size (*64 bytes) */
0138: #define SINCR   20              /* increment of stack (*64 bytes) */
0139: #define NOFILE  15              /* max open files per process */
0140: #define CANBSIZ 256             /* max size of typewriter line */
0141: #define CMAPSIZ 100             /* size of core allocation area */
0142: #define SMAPSIZ 100             /* size of swap allocation area */
0143: #define NCALL   20              /* max simultaneous time callouts */
0144: #define NPROC   50              /* max number of processes */
0145: #define NTEXT   40              /* max number of pure texts */
0146: #define NCLIST  100             /* max total clist size */
0147: #define HZ      60              /* Ticks/second of the clock */
0148: 
0149: 
0150: 
0151: /* priorities: probably should not be altered too much */
0152: 
0153: 
0154: #define PSWP    -100
0155: #define PINOD   -90
0156: #define PRIBIO  -50
0157: #define PPIPE   1
0158: #define PWAIT   40
0159: #define PSLEP   90
0160: #define PUSER   100
0161: 
0162: /* Certain processor registers */
0163: 
0164: #define PS      0177776
0165: #define KL      0177560
0166: #define SW      0177570
0167: 
0168: /* ---------------------------       */
0169: 
0170: /* structure to access : */
0171: 
0172: 
0173:    /* an integer */
0174: 
0175: struct {   int   integ;   };
0176: 
0177: 
0178:    /* an integer in bytes */
0179: 
0180: struct {   char lobyte;   char hibyte;   };
0181: 
0182: 
0183:    /* a sequence of integers */
0184: 
0185: struct {   int   r[];   };
0186: 
0187: 
0188: /* ---------------------------       */

0200: /* Random set of variables used by more than one routine. */
0201: 
0202: char    canonb[CANBSIZ];        /* buffer for erase and kill (#@) */
0203: int     coremap[CMAPSIZ];       /* space for core allocation */
0204: int     swapmap[SMAPSIZ];       /* space for swap allocation */
0205: 
0206: int     *rootdir;               /* pointer to inode of root directory */
0207: 
0208: int     cputype;                /* type of cpu =40, 45, or 70 */
0209: 
0210: int     execnt;                 /* number of processes in exec */
0211: 
0212: int     lbolt;                  /* time of day in 60th not in time */
0213: int     time[2];                /* time in sec from 1970 */
0214: int     tout[2];                /* time of day of next sleep */
0215: 
0216: int     mpid;                   /* generic for unique process id's */
0217: 
0218: char    runin;                  /* scheduling flag */
0219: char    runout;                 /* scheduling flag */
0220: char    runrun;                 /* scheduling flag */
0221: 
0222: char    curpri;                 /* more scheduling */
0223: 
0224: int     maxmem;                 /* actual max memory per process */
0225: 
0226: int     *lks;                   /* pointer to clock device */
0227: 
0228: int     rootdev;                /* dev of root see conf.c */
0229: int     swapdev;                /* dev of swap see conf.c */
0230: 
0231: int     swplo;                  /* block number of swap space */
0232: int     nswap;                  /* size of swap space */
0233: 
0234: int     updlock;                /* lock for sync */
0235: int     rablock;                /* block to be read ahead */
0236: 
0237: char    regloc[];               /* locs. of saved user registers (trap.c) */
0238: 
0239: 
0240: 
0241: /* ---------------------------       */
0242: 
0243: 
0244: 
0245: 
0246: 
0247: 
0248: 
0249: 
0250: 
0251: /* ---------------------------       */
0252: 
0253: /* The callout structure is for a routine
0254:  * arranging to be called by the clock interrupt
0255:  * (clock.c) with a specified argument,
0256:  * within a specified amount of time.
0257:  * It is used, for example, to time tab delays
0258:  * on teletypes. */
0259: 
0260: struct  callo
0261: {
0262:         int     c_time;         /* incremental time */
0263:         int     c_arg;          /* argument to routine */
0264:         int     (*c_func)();    /* routine */
0265: } callout[NCALL];
0266: /* ---------------------------       */
0267: 
0268: /* Mount structure.
0269:  * One allocated on every mount. Used to find the super block.
0270:  */
0271: 
0272: struct  mount
0273: {
0274:         int     m_dev;          /* device mounted */
0275:         int     *m_bufp;        /* pointer to superblock */
0276:         int     *m_inodp;       /* pointer to mounted on inode */
0277: } mount[NMOUNT];
0278: /* ---------------------------       */

0300: 
0301: /* KT-11 addresses and bits */
0302: 
0303: #define UISD    0177600         /* first user I-space descriptor register */
0304: 
0305: #define UISA    0177640         /* first user I-space address register */
0306: 
0307: #define UDSA    0177660         /* first user D-space address register */
0308: 
0309: 
0310: #define UBMAP   0170200         /* address to access 11/70 UNIBUS map */
0311: 
0312: #define RO      02              /* access abilities */
0313: #define WO      04
0314: #define RW      06
0315: #define ED      010             /* extend direction */
0316: 
0317: /* ---------------------------       */
0318: 
0319: int     *ka6;           /* 11/40 KISA6; 11/45 KDSA6 */

0350: /*
0351:  * One structure allocated per active
0352:  * process. It contains all data needed
0353:  * about the process while the
0354:  * process may be swapped out.
0355:  * Other per process data (user.h)
0356:  * is swapped with the process.
0357:  */
0358: struct  proc
0359: {
0360:         char    p_stat;
0361:         char    p_flag;
0362:         char    p_pri;          /* priority, negative is high */
0363:         char    p_sig;          /* signal number sent to this process */
0364:         char    p_uid;          /* user id, used to direct tty signals */
0365:         char    p_time;         /* resident time for scheduling */
0366:         char    p_cpu;          /* cpu usage for scheduling */
0367:         char    p_nice;         /* nice for scheduling */
0368:         int     p_ttyp;         /* controlling tty */
0369:         int     p_pid;          /* unique process id */
0370:         int     p_ppid;         /* process id of parent */
0371:         int     p_addr;         /* address of swappable image */
0372:         int     p_size;         /* size of swappable image (*64 bytes) */
0373:         int     p_wchan;        /* event process is awaiting */
0374:         int     *p_textp;       /* pointer to text structure */
0375: 
0376: } proc[NPROC];
0377: /* ---------------------------       */
0378: 
0379: /* stat codes */
0380: 
0381: 
0382: #define SSLEEP  1               /* sleeping on high priority */
0383: #define SWAIT   2               /* sleeping on low priority */
0384: #define SRUN    3               /* running */
0385: #define SIDL    4               /* intermediate state in process creation */
0386: #define SZOMB   5               /* intermediate state in process termination */
0387: #define SSTOP   6               /* process being traced */
0388: 
0389: /* flag codes */
0390: 
0391: #define SLOAD   01              /* in core */
0392: #define SSYS    02              /* scheduling process */
0393: #define SLOCK   04              /* process cannot be swapped */
0394: #define SSWAP   010             /* process is being swapped out */
0395: #define STRC    020             /* process is being traced */
0396: #define SWTED   040             /* another tracing flag */

0400: /*
0401:  * The user structure.
0402:  * One allocated per process.
0403:  * Contains all per process data
0404:  * that doesn't need to be referenced
0405:  * while the process is swapped.
0406:  * The user block is USIZE*64 bytes
0407:  * long; resides at virtual kernel
0408:  * loc 140000; contains the system
0409:  * stack per user; is cross referenced
0410:  * with the proc structure for the
0411:  * same process.
0412:  */
0413: struct user
0414: {
0415:         int     u_rsav[2];              /* save r5,r6 when exchanging stacks */
0416:         int     u_fsav[25];             /* save fp registers */
0417:                                         /* rsav and fsav must be first in structure */
0418:         char    u_segflg;               /* flag for IO; user or kernel space */
0419:         char    u_error;                /* return error code */
0420:         char    u_uid;                  /* effective user id */
0421:         char    u_gid;                  /* effective group id */
0422:         char    u_ruid;                 /* real user id */
0423:         char    u_rgid;                 /* real group id */
0424:         int     u_procp;                /* pointer to proc structure */
0425:         char    *u_base;                /* base address for IO */
0426:         char    *u_count;               /* bytes remaining for IO */
0427:         char    *u_offset[2];           /* offset in file for IO */
0428:         int     *u_cdir;                /* pointer to inode of current directory */
0429:         char    u_dbuf[DIRSIZ];         /* current pathname component */
0430:         char    *u_dirp;                /* current pointer to inode */
0431:         struct  {                       /* current directory entry */
0432:                 int     u_ino;
0433:                 char    u_name[DIRSIZ];
0434:         } u_dent;
0435:         int     *u_pdir;                /* inode of parent directory of dirp */
0436:         int     u_uisa[16];             /* prototype of segmentation addresses */
0437:         int     u_uisd[16];             /* prototype of segmentation descriptors */
0438:         int     u_ofile[NOFILE];        /* pointers to file structures of open files */
0439:         int     u_arg[5];               /* arguments to current system call */
0440:         int     u_tsize;                /* text size (*64) */
0441:         int     u_dsize;                /* data size (*64) */
0442:         int     u_ssize;                /* stack size (*64) */
0443:         int     u_sep;                  /* flag for I and D separation */
0444:         int     u_qsav[2];              /* label variable for quits and interrupts */
0445:         int     u_ssav[2];              /* label variable for swapping */
0446:         int     u_signal[NSIG];         /* disposition of signals */
0447:         int     u_utime;                /* this process user time */
0448:         int     u_stime;                /* this process system time */
0449:         int     u_cutime[2];            /* sum of childs' utimes */
0450:         int     u_cstime[2];            /* sum of childs' stimes */
0451:         int     *u_ar0;                 /* address of users saved R0 */
0452:         int     u_prof[4];              /* profile arguments */
0453:         char    u_intflg;               /* catch intr from sys */
0454:                                         /* kernel stack per user
0455:                                          * extends from u + USIZE*64
0456:                                          * backward not to reach here
0457:                                          */
0458: } u;
0459: /* ---------------------------       */
0460: 
0461: /* u_error codes */
0462: 
0463: 
0464: 
0465: #define EFAULT  106
0466: #define EPERM   1
0467: #define ENOENT  2
0468: #define ESRCH   3
0469: #define EINTR   4
0470: #define EIO     5
0471: #define ENXIO   6
0472: #define E2BIG   7
0473: #define ENOEXEC 8
0474: #define EBADF   9
0475: #define ECHILD  10
0476: #define EAGAIN  11
0477: #define ENOMEM  12
0478: #define EACCES  13
0479: #define ENOTBLK 15
0480: #define EBUSY   16
0481: #define EEXIST  17
0482: #define EXDEV   18
0483: #define ENODEV  19
0484: #define ENOTDIR 20
0485: #define EISDIR  21
0486: #define EINVAL  22
0487: #define ENFILE  23
0488: #define EMFILE  24
0489: #define ENOTTY  25
0490: #define ETXTBSY 26
0491: #define EFBIG   27
0492: #define ENOSPC  28
0493: #define ESPIPE  29
0494: #define EROFS   30
0495: #define EMLINK  31
0496: #define EPIPE   32

0500: / low core
0501: 
0502: br4 = 200
0503: br5 = 240
0504: br6 = 300
0505: br7 = 340
0506: 
0507: . = 0^.
0508:         br      1f
0509:         4
0510: 
0511: / trap vectors
0512:         trap; br7+0.            / bus error
0513:         trap; br7+1.            / illegal instruction
0514:         trap; br7+2.            / bpt-trace trap
0515:         trap; br7+3.            / iot trap
0516:         trap; br7+4.            / power fail
0517:         trap; br7+5.            / emulator trap
0518:         trap; br7+6.            / system entry
0519: 
0520: . = 40^.
0521: .globl  start, dump
0522: 1:      jmp     start
0523:         jmp     dump
0524: 
0525: . = 60^.
0526:         klin; br4
0527:         klou; br4
0528: 
0529: . = 70^.
0530:         pcin; br4
0531:         pcou; br4
0532: 
0533: . = 100^.
0534:         kwlp; br6
0535:         kwlp; br6
0536: 
0537: . = 114^.
0538:         trap; br7+7.            / 11/70 parity
0539: 
0540: . = 200^.
0541:         lpou; br4
0542: 
0543: . = 220^.
0544:         rkio; br5
0545: 
0546: . = 240^.
0547:         trap; br7+7.            / programmed interrupt
0548:         trap; br7+8.            / floating point
0549:         trap; br7+9.            / segmentation violation
0550: 
0551: //////////////////////////////////////////////////////
0552: /               interface code to C
0553: //////////////////////////////////////////////////////
0554: 
0555: .globl  call, trap
0556: 
0557: .globl  _klrint
0558: klin:   jsr     r0,call; _klrint
0559: 
0560: .globl  _klxint
0561: klou:   jsr     r0,call; _klxint
0562: 
0563: .globl  _pcrint
0564: pcin:   jsr     r0,call; _pcrint
0565: 
0566: .globl  _pcpint
0567: pcou:   jsr     r0,call; _pcpint
0568: 
0569: .globl  _clock
0570: kwlp:   jsr     r0,call; _clock
0571: 
0572: 
0573: .globl  _lpintr
0574: lpou:   jsr     r0,call; _lpintr
0575: 
0576: .globl  _rkintr
0577: rkio:   jsr     r0,call; _rkintr

0600: / machine language assist
0601: / for 11/40
0602: 
0603: / non-UNIX instructions
0604: mfpi    = 6500^tst
0605: mtpi    = 6600^tst
0606: wait    = 1
0607: rtt     = 6
0608: reset   = 5
0609: 
0610: /* ---------------------------       */
0611: .globl  start, _end, _edata, _main
0612: start:
0613:         bit     $1,SSR0
0614:         bne     start                   / loop if restart
0615:         reset
0616: 
0617: / initialize systems segments
0618: 
0619:         mov     $KISA0,r0
0620:         mov     $KISD0,r1
0621:         mov     $200,r4
0622:         clr     r2
0623:         mov     $6,r3
0624: 1:
0625:         mov     r2,(r0)+
0626:         mov     $77406,(r1)+            / 4k rw
0627:         add     r4,r2
0628:         sob     r3,1b
0629: 
0630: / initialize user segment
0631: 
0632:         mov     $_end+63.,r2
0633:         ash     $-6,r2
0634:         bic     $!1777,r2
0635:         mov     r2,(r0)+                / ksr6 = sysu
0636:         mov     $usize-1\<8|6,(r1)+
0637: 
0638: / initialize io segment
0639: / set up counts on supervisor segments
0640: 
0641:         mov     $IO,(r0)+
0642:         mov     $77406,(r1)+            / rw 4k
0643: 
0644: / get a sp and start segmentation
0645: 
0646:         mov     $_u+[usize*64.],sp
0647:         inc     SSR0
0648: 
0649: / clear bss
0650: 
0651:         mov     $_edata,r0
0652: 1:
0653:         clr     (r0)+
0654:         cmp     r0,$_end
0655:         blo     1b
0656: 
0657: / clear user block
0658: 
0659:         mov     $_u,r0
0660: 1:
0661:         clr     (r0)+
0662:         cmp     r0,$_u+[usize*64.]
0663:         blo     1b
0664: 
0665: / set up previous mode and call main
0666: / on return, enter user mode at 0R
0667: 
0668:         mov     $30000,PS
0669:         jsr     pc,_main
0670:         mov     $170000,-(sp)
0671:         clr     -(sp)
0672:         rtt
0673: 
0674: /* ---------------------------       */
0675: .globl  _clearseg
0676: _clearseg:
0677:         mov     PS,-(sp)
0678:         mov     UISA0,-(sp)
0679:         mov     $30340,PS
0680:         mov     6(sp),UISA0
0681:         mov     UISD0,-(sp)
0682:         mov     $6,UISD0
0683:         clr     r0
0684:         mov     $32.,r1
0685: 1:
0686:         clr     -(sp)
0687:         mtpi    (r0)+
0688:         sob     r1,1b
0689:         mov     (sp)+,UISD0
0690:         mov     (sp)+,UISA0
0691:         mov     (sp)+,PS
0692:         rts     pc
0693: 
0694: /* ---------------------------       */
0695: .globl  _copyseg
0696: _copyseg:
0697:         mov     PS,-(sp)
0698:         mov     UISA0,-(sp)
0699:         mov     UISA1,-(sp)
0700:         mov     $30340,PS
0701:         mov     10(sp),UISA0
0702:         mov     12(sp),UISA1
0703:         mov     UISD0,-(sp)
0704:         mov     UISD1,-(sp)
0705:         mov     $6,UISD0
0706:         mov     $6,UISD1
0707:         mov     r2,-(sp)
0708:         clr     r0
0709:         mov     $8192.,r1
0710:         mov     $32.,r2
0711: 1:
0712:         mfpi    (r0)+
0713:         mtpi    (r1)+
0714:         sob     r2,1b
0715:         mov     (sp)+,r2
0716:         mov     (sp)+,UISD1
0717:         mov     (sp)+,UISD0
0718:         mov     (sp)+,UISA1
0719:         mov     (sp)+,UISA0
0720:         mov     (sp)+,PS
0721:         rts     pc
0722: 
0723: /* ---------------------------       */
0724: .globl  _savu, _retu, _aretu
0725: _savu:
0726:         bis     $340,PS
0727:         mov     (sp)+,r1
0728:         mov     (sp),r0
0729:         mov     sp,(r0)+
0730:         mov     r5,(r0)+
0731:         bic     $340,PS
0732:         jmp     (r1)
0733: 
0734: _aretu:
0735:         bis     $340,PS
0736:         mov     (sp)+,r1
0737:         mov     (sp),r0
0738:         br      1f
0739: 
0740: _retu:
0741:         bis     $340,PS
0742:         mov     (sp)+,r1
0743:         mov     (sp),KISA6
0744:         mov     $_u,r0
0745: 1:
0746:         mov     (r0)+,sp
0747:         mov     (r0)+,r5
0748:         bic     $340,PS
0749:         jmp     (r1)
0750: 
0751: /* ---------------------------       */
0752: .globl  trap, call
0753: /* ---------------------------       */
0754: .globl  _trap
0755: trap:
0756:         mov     PS,-4(sp)
0757:         tst     nofault
0758:         bne     1f
0759:         mov     SSR0,ssr
0760:         mov     SSR2,ssr+4
0761:         mov     $1,SSR0
0762:         jsr     r0,call1; _trap
0763:         / no return
0764: 1:
0765:         mov     $1,SSR0
0766:         mov     nofault,(sp)
0767:         rtt
0768: 
0769: /* ---------------------------       */
0770: .globl  _runrun, _swtch
0771: call1:
0772:         tst     -(sp)
0773:         bic     $340,PS
0774:         br      1f
0775: 
0776: call:
0777:         mov     PS,-(sp)
0778: 1:
0779:         mov     r1,-(sp)
0780:         mfpi    sp
0781:         mov     4(sp),-(sp)
0782:         bic     $!37,(sp)
0783:         bit     $30000,PS
0784:         beq     1f
0785:         jsr     pc,*(r0)+
0786: 2:
0787:         bis     $340,PS
0788:         tstb    _runrun
0789:         beq     2f
0790:         bic     $340,PS
0791:         jsr     pc,_swtch
0792:         br      2b
0793: 2:
0794:         tst     (sp)+
0795:         mtpi    sp
0796:         br      2f
0797: 1:
0798:         bis     $30000,PS
0799:         jsr     pc,*(r0)+
0800:         cmp     (sp)+,(sp)+
0801: 2:
0802:         mov     (sp)+,r1
0803:         tst     (sp)+
0804:         mov     (sp)+,r0
0805:         rtt
0806: /* ---------------------------       */
0807: .globl  _fubyte, _subyte
0808: /* ---------------------------       */
0809: .globl  _fuibyte, _suibyte
0810: /* ---------------------------       */
0811: .globl  _fuword, _suword
0812: /* ---------------------------       */
0813: .globl  _fuiword, _suiword
0814: _fuibyte:
0815: _fubyte:
0816:         mov     2(sp),r1
0817:         bic     $1,r1
0818:         jsr     pc,gword
0819:         cmp     r1,2(sp)
0820:         beq     1f
0821:         swab    r0
0822: 1:
0823:         bic     $!377,r0
0824:         rts     pc
0825: 
0826: _suibyte:
0827: _subyte:
0828:         mov     2(sp),r1
0829:         bic     $1,r1
0830:         jsr     pc,gword
0831:         mov     r0,-(sp)
0832:         cmp     r1,4(sp)
0833:         beq     1f
0834:         movb    6(sp),1(sp)
0835:         br      2f
0836: 1:
0837:         movb    6(sp),(sp)
0838: 2:
0839:         mov     (sp)+,r0
0840:         jsr     pc,pword
0841:         clr     r0
0842:         rts     pc
0843: 
0844: _fuiword:
0845: _fuword:
0846:         mov     2(sp),r1
0847: fuword:
0848:         jsr     pc,gword
0849:         rts     pc
0850: 
0851: gword:
0852:         mov     PS,-(sp)
0853:         bis     $340,PS
0854:         mov     nofault,-(sp)
0855:         mov     $err,nofault
0856:         mfpi    (r1)
0857:         mov     (sp)+,r0
0858:         br      1f
0859: 
0860: _suiword:
0861: _suword:
0862:         mov     2(sp),r1
0863:         mov     4(sp),r0
0864: suword:
0865:         jsr     pc,pword
0866:         rts     pc
0867: 
0868: pword:
0869:         mov     PS,-(sp)
0870:         bis     $340,PS
0871:         mov     nofault,-(sp)
0872:         mov     $err,nofault
0873:         mov     r0,-(sp)
0874:         mtpi    (r1)
0875: 1:
0876:         mov     (sp)+,nofault
0877:         mov     (sp)+,PS
0878:         rts     pc
0879: 
0880: err:
0881:         mov     (sp)+,nofault
0882:         mov     (sp)+,PS
0883:         tst     (sp)+
0884:         mov     $-1,r0
0885:         rts     pc
0886: 
0887: /* ---------------------------       */
0888: .globl  _savfp, _display
0889: _savfp:
0890: _display:
0891:         rts     pc
0892: 
0893: /* ---------------------------       */
0894: .globl  _incupc
0895: _incupc:
0896:         mov     r2,-(sp)
0897:         mov     6(sp),r2        / base of prof with base,leng,off,scale
0898:         mov     4(sp),r0        / pc
0899:         sub     4(r2),r0        / offset
0900:         clc
0901:         ror     r0
0902:         mul     6(r2),r0        / scale
0903:         ashc    $-14.,r0
0904:         inc     r1
0905:         bic     $1,r1
0906:         cmp     r1,2(r2)        / length
0907:         bhis    1f
0908:         add     (r2),r1         / base
0909:         mov     nofault,-(sp)
0910:         mov     $2f,nofault
0911:         mfpi    (r1)
0912:         inc     (sp)
0913:         mtpi    (r1)
0914:         br      3f
0915: 2:
0916:         clr     6(r2)
0917: 3:
0918:         mov     (sp)+,nofault
0919: 1:
0920:         mov     (sp)+,r2
0921:         rts     pc
0922: 
0923: / Character list get/put
0924: 
0925: /* ---------------------------       */
0926: .globl  _getc, _putc
0927: /* ---------------------------       */
0928: .globl  _cfreelist
0929: 
0930: _getc:
0931:         mov     2(sp),r1
0932:         mov     PS,-(sp)
0933:         mov     r2,-(sp)
0934:         bis     $340,PS
0935:         bic     $100,PS         / spl 5
0936:         mov     2(r1),r2        / first ptr
0937:         beq     9f              / empty
0938:         movb    (r2)+,r0        / character
0939:         bic     $!377,r0
0940:         mov     r2,2(r1)
0941:         dec     (r1)+           / count
0942:         bne     1f
0943:         clr     (r1)+
0944:         clr     (r1)+           / last block
0945:         br      2f
0946: 1:
0947:         bit     $7,r2
0948:         bne     3f
0949:         mov     -10(r2),(r1)    / next block
0950:         add     $2,(r1)
0951: 2:
0952:         dec     r2
0953:         bic     $7,r2
0954:         mov     _cfreelist,(r2)
0955:         mov     r2,_cfreelist
0956: 3:
0957:         mov     (sp)+,r2
0958:         mov     (sp)+,PS
0959:         rts     pc
0960: 9:
0961:         clr     4(r1)
0962:         mov     $-1,r0
0963:         mov     (sp)+,r2
0964:         mov     (sp)+,PS
0965:         rts     pc
0966: 
0967: _putc:
0968:         mov     2(sp),r0
0969:         mov     4(sp),r1
0970:         mov     PS,-(sp)
0971:         mov     r2,-(sp)
0972:         mov     r3,-(sp)
0973:         bis     $340,PS
0974:         bic     $100,PS         / spl 5
0975:         mov     4(r1),r2        / last ptr
0976:         bne     1f
0977:         mov     _cfreelist,r2
0978:         beq     9f
0979:         mov     (r2),_cfreelist
0980:         clr     (r2)+
0981:         mov     r2,2(r1)        / first ptr
0982:         br      2f
0983: 1:
0984:         bit     $7,r2
0985:         bne     2f
0986:         mov     _cfreelist,r3
0987:         beq     9f
0988:         mov     (r3),_cfreelist
0989:         mov     r3,-10(r2)
0990:         mov     r3,r2
0991:         clr     (r2)+
0992: 2:
0993:         movb    r0,(r2)+
0994:         mov     r2,4(r1)
0995:         inc     (r1)            / count
0996:         clr     r0
0997:         mov     (sp)+,r3
0998:         mov     (sp)+,r2
0999:         mov     (sp)+,PS
1000:         rts     pc
1001: 9:
1002:         mov     pc,r0
1003:         mov     (sp)+,r3
1004:         mov     (sp)+,r2
1005:         mov     (sp)+,PS
1006:         rts     pc
1007: 
1008: /* ---------------------------       */
1009: .globl  _backup
1010: /* ---------------------------       */
1011: .globl  _regloc
1012: _backup:
1013:         mov     2(sp),ssr+2
1014:         mov     r2,-(sp)
1015:         jsr     pc,backup
1016:         mov     r2,ssr+2
1017:         mov     (sp)+,r2
1018:         movb    jflg,r0
1019:         bne     2f
1020:         mov     2(sp),r0
1021:         movb    ssr+2,r1
1022:         jsr     pc,1f
1023:         movb    ssr+3,r1
1024:         jsr     pc,1f
1025:         movb    _regloc+7,r1
1026:         asl     r1
1027:         add     r0,r1
1028:         mov     ssr+4,(r1)
1029:         clr     r0
1030: 2:
1031:         rts     pc
1032: 1:
1033:         mov     r1,-(sp)
1034:         asr     (sp)
1035:         asr     (sp)
1036:         asr     (sp)
1037:         bic     $!7,r1
1038:         movb    _regloc(r1),r1
1039:         asl     r1
1040:         add     r0,r1
1041:         sub     (sp)+,(r1)
1042:         rts     pc
1043: 
1044: / hard part
1045: / simulate the ssr2 register missing on 11/40
1046: 
1047: backup:
1048:         clr     r2              / backup register ssr1
1049:         mov     $1,bflg         / clrs jflg
1050:         mov     ssr+4,r0
1051:         jsr     pc,fetch
1052:         mov     r0,r1
1053:         ash     $-11.,r0
1054:         bic     $!36,r0
1055:         jmp     *0f(r0)
1056: 0:              t00; t01; t02; t03; t04; t05; t06; t07
1057:                 t10; t11; t12; t13; t14; t15; t16; t17
1058: 
1059: t00:
1060:         clrb    bflg
1061: 
1062: t10:
1063:         mov     r1,r0
1064:         swab    r0
1065:         bic     $!16,r0
1066:         jmp     *0f(r0)
1067: 0:              u0; u1; u2; u3; u4; u5; u6; u7
1068: 
1069: u6:     / single op, m[tf]pi, sxt, illegal
1070:         bit     $400,r1
1071:         beq     u5              / all but m[tf], sxt
1072:         bit     $200,r1
1073:         beq     1f              / mfpi
1074:         bit     $100,r1
1075:         bne     u5              / sxt
1076: 
1077: / simulate mtpi with double (sp)+,dd
1078:         bic     $4000,r1        / turn instr into (sp)+
1079:         br      t01
1080: 
1081: / simulate mfpi with double ss,-(sp)
1082: 1:
1083:         ash     $6,r1
1084:         bis     $46,r1          / -(sp)
1085:         br      t01
1086: 
1087: u4:     / jsr
1088:         mov     r1,r0
1089:         jsr     pc,setreg       / assume no fault
1090:         bis     $173000,r2      / -2 from sp
1091:         rts     pc
1092: 
1093: t07:    / EIS
