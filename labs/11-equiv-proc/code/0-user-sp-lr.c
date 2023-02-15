// some simple tests to make really sure that USER banked registers and
// instructions we need for switching USER level processes work
// as expected:
//   - ldm ^ (with one and multiple registers)
//   - stm ^ (with one and multiple registers)
//   - cps to SYSTEM and back.
//
// basic idea: write known values, read back and make sure they are 
// as expected.
//
// [should rewrite so that this uses randomized values for some number of
// times.]
#include "rpi.h"

// in support-asm.S

// get USER <sp> by using <cps> to switch to SYSTEM
// and back.  
uint32_t user_sp_get(void);
void user_sp_set(uint32_t sp);

// get USER <lr> by using <cps> to switch to SYSTEM
// and back.
uint32_t user_lr_get(void);
void user_lr_set(uint32_t lr);

// use ldm/stm with the ^ modifier to get USER mode <sp>
void mem_user_sp_get(uint32_t *sp);
void mem_user_sp_set(const uint32_t *sp);

// use ldm/stm with the ^ modifier to get USER mode <lr>
void mem_user_lr_get(uint32_t *sp);
void mem_user_lr_set(const uint32_t *sp);

// use ldm/stm with the ^ modifier to get USER 
// mode <lr> and <sp>
void mem_user_sp_lr_get(uint32_t splr[2]);
void mem_user_sp_lr_set(const uint32_t splr[2]);

void notmain(void) {
    // both <sp> and <lr> values are just whatever garbage
    // is there on bootup.  i'm honestly a bit surprised 
    // it's not <0> (at least on my r/pi).
    output("uninitialized user sp=%x\n", user_sp_get());
    output("uninitialized user lr=%x\n", user_lr_get());
    
    /*********************************************************
     * part 1: _set/_get USER sp,lr using the <cps> method 
     */
    // set the registers to traditional OS weird values.
    trace("-----------------------------------------------------------\n");
    trace("part 1: get/set USER sp/lr by switching to SYSTEM and back\n");
    enum { SP = 0xfeedface, LR = 0xdeadbeef};
    uint32_t sp_set = SP, lr_set = LR;
    user_sp_set(sp_set);  // user sp should be == SP
    user_lr_set(lr_set);  // user lr should be == LR

    // make sure what we *_set is what we *_get()
    uint32_t sp_get = user_sp_get();
    uint32_t lr_get = user_lr_get();
    trace("\tgot USER sp=%x\n", sp_get);
    trace("\tgot USER lr=%x\n", lr_get);
    assert(sp_get == SP);
    assert(lr_get == LR);
    trace("part 1: passed\n");

    // reset USER sp/lr to something else so we can do another
    // test.
    user_sp_set(0);
    user_lr_set(0);
    assert(user_sp_get() == 0);
    assert(user_lr_get() == 0);

    /*********************************************************
     * part 2: _set/_get USER sp,lr using the ldm/stm versions.
     */
    trace("-----------------------------------------------------------\n");
    trace("part 2: set/get USER sp/lr using ldm/stm with ^\n");

    // set them to known values.
    sp_set = SP;
    lr_set = LR;
    mem_user_sp_set(&sp_set);       
    mem_user_lr_set(&lr_set);
    // read back 
    mem_user_sp_get(&sp_get);       
    mem_user_lr_get(&lr_get);
    trace("\tgot USER sp=%x\n", sp_get);
    trace("\tgot USER lr=%x\n", lr_get);
    assert(sp_get == SP);
    assert(lr_get == LR);
    // check previous operations still work, too, twice.
    assert(sp_get == user_sp_get());
    assert(lr_get == user_lr_get());

    assert(sp_get == user_sp_get());  // nothing should change
    assert(lr_get == user_lr_get());
    trace("part 2 passed\n");

    // reset USER sp/lr to something else so we can do another
    // test.
    user_sp_set(0);
    user_lr_set(0);
    assert(user_sp_get() == 0);
    assert(user_lr_get() == 0);

    /*********************************************************
     * part 3: _set/_get USER sp,lr using the double
     * memory ldm/stm versions.
     */
    trace("-----------------------------------------------------------\n");
    trace("part 3: set/get USER sp/lr using single ldm/stm ^ instruction\n");
    const uint32_t splr_set[2] = { SP, LR };
    uint32_t splr_get[2];
    mem_user_sp_lr_set(splr_set);
    mem_user_sp_lr_get(splr_get);
    trace("\tgot USER sp=%x\n", splr_get[0]);
    trace("\tgot USER lr=%x\n", splr_get[0]);
    assert(SP == splr_get[0]);
    assert(LR == splr_get[1]);

    // double check we get the same thing on a second invocation.
    splr_get[0] = splr_get[1] = 0;
    mem_user_sp_lr_get(splr_get);
    assert(SP == splr_get[0]);
    assert(LR == splr_get[1]);

    trace("part 3: passed\n");

    output("SUCCESS: all tests passed.\n");
}
