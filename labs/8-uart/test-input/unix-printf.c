

2330: /*
2331:  * Scaled down version of C Library printf.
2332:  * Only %s %l %d (==%l) %o are recognized.
2333:  * Used to print diagnostic information
2334:  * directly on console tty.
2335:  * Since it is not interrupt driven,
2336:  * all system activities are pretty much
2337:  * suspended.
2338:  * Printf should not be used for chit-chat.
2339:  */
2340: printf(fmt,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc)
2341: char fmt[];
2342: {
2343:         register char *s;
2344:         register *adx, c;
2345: 
2346:         adx = &x1;
2347: loop:
2348:         while((c = *fmt++) != '%') {
2349:                 if(c == '\0')
2350:                         return;
2351:                 putchar(c);
2352:         }
2353:         c = *fmt++;
2354:         if(c == 'd' || c == 'l' || c == 'o')
2355:                 printn(*adx, c=='o'? 8: 10);
2356:         if(c == 's') {
2357:                 s = *adx;
2358:                 while(c = *s++)
2359:                         putchar(c);
2360:         }
2361:         adx++;
2362:         goto loop;
2363: }
2364: /* ---------------------------       */
2365: 
2366: /*
2367:  * Print an unsigned integer in base b.
2368:  */
2369: printn(n, b)
2370: {
2371:         register a;
2372: 
2373:         if(a = ldiv(n, b))
2374:                 printn(a, b);
2375:         putchar(lrem(n, b) + '0');
2376: }
