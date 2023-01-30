/*                  DO NOT MODIFY THIS CODE.
 *                  DO NOT MODIFY THIS CODE.
 *                  DO NOT MODIFY THIS CODE.
 *                  DO NOT MODIFY THIS CODE.
 *
 * engler, cs140e: trivial driver for your get_code implementation,
 * which does the actual getting and loading of the code.
 */
#include "rpi.h"
#include "get-code.h"

void notmain(void) {
    uint32_t addr;
    if(!get_code(&addr))
        rpi_reboot();
    BRANCHTO(addr);
    not_reached();
}
