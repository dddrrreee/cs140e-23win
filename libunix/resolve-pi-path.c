// look up the given pi binary in (1) the current directory, or (2) in 
// the directories listed in PI_PATH (right now: must be 1).
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "libunix.h"

#define _SVID_SOURCE
#include <dirent.h>

// lookup <name> in directory <path>, 
// returns 
//  -  "<path>/<name>" on match.
//  -  0 otherwise.
char *name_lookup(const char *path, const char *name) {
    unimplemented();
}

// look for a pi binary in ./ or colon-seperated list in
// <PI_PATH> 
const char *find_pi_binary(const char *name) {
    // if the given name exists, just return.
    if(exists(name))
        return strdup(name);

    unimplemented();
}
