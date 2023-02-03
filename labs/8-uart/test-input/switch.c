0000: // snippet from ken thompson's original unix code.
0000: //
0000: //    from lion's commentary: https://warsus.github.io/lions-/
0000: 
2172: /*
2173:  * This routine is called to reschedule the CPU.
2174:  * if the calling process is not in RUN state,
2175:  * arrangements for it to restart must have
2176:  * been made elsewhere, usually by calling via sleep.
2177:  */
2178: swtch()
2179: {
2180:         static struct proc *p;
2181:         register i, n;
2182:         register struct proc *rp;
2183: 
2184:         if(p == NULL)
2185:                 p = &proc[0];
2186:         /*
2187:          * Remember stack of caller
2188:          */
2189:         savu(u.u_rsav);
2190:         /*
2191:          * Switch to scheduler's stack
2192:          */
2193:         retu(proc[0].p_addr);
2194: 
2195: loop:
2196:         runrun = 0;
2197:         rp = p;
2198:         p = NULL;
2199:         n = 128;
2200:         /*
2201:          * Search for highest-priority runnable process
2202:          */
2203:         i = NPROC;
2204:         do {
2205:                 rp++;
2206:                 if(rp >= &proc[NPROC])
2207:                         rp = &proc[0];
2208:                 if(rp->p_stat==SRUN && (rp->p_flag&SLOAD)!=0) {
2209:                         if(rp->p_pri < n) {
2210:                                 p = rp;
2211:                                 n = rp->p_pri;
2212:                         }
2213:                 }
2214:         } while(--i);
2215:         /*
2216:          * If no process is runnable, idle.
2217:          */
2218:         if(p == NULL) {
2219:                 p = rp;
2220:                 idle();
2221:                 goto loop;
2222:         }
2223:         rp = p;
2224:         curpri = n;
2225:         /* Switch to stack of the new process and set up
2226:          * his segmentation registers.
2227:          */
2228:         retu(rp->p_addr);
2229:         sureg();
2230:         /*
2231:          * If the new process paused because it was
2232:          * swapped out, set the stack level to the last call
2233:          * to savu(u_ssav).  This means that the return
2234:          * which is executed immediately after the call to aretu
2235:          * actually returns from the last routine which did
2236:          * the savu.
2237:          *
2238:          * You are not expected to understand this.
2239:          */
2240:         if(rp->p_flag&SSWAP) {
2241:                 rp->p_flag =& ~SSWAP;
2242:                 aretu(u.u_ssav);
2243:         }
2244:         /* The value returned here has many subtle implications.
2245:          * See the newproc comments.
2246:          */
2247:         return(1);
2248: }
