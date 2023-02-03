// engler, cs140e: your code to find the tty-usb device on your laptop.
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>


#include "libunix.h"

#define _SVID_SOURCE
#include <dirent.h>


static const char *ttyusb_prefixes[] = {
	"ttyUSB",	// linux
    "ttyACM",   // linux
	"cu.SLAB_USB", // mac os
    //"cu.usbserial", // mac os
    // if your system uses another name, add it.
	0
};

static int filter(const struct dirent *d) {
    for (int i = 0; ttyusb_prefixes[i]; i++) {
        if (strncmp(d->d_name, ttyusb_prefixes[i], strlen(ttyusb_prefixes[i])) == 0) {
            return 1;
        }
    }
    return 0;
    // scan through the prefixes, returning 1 when you find a match.
    // 0 if there is no match.
    //unimplemented();
}

// find the TTY-usb device (if any) by using <scandir> to search for
// a device with a prefix given by <ttyusb_prefixes> in /dev
// returns:
//  - device name.
// error: panic's if 0 or more than 1 devices.
char *find_ttyusb(void) {
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, filter, alphasort);
    if (n < 0) panic("scandir");
    if (n != 1) panic("more than one tty-usb device found.");



    char *name = strdupf(namelist[0]->d_name);
    char *full_path = (char *) malloc(strlen(name) + strlen("/dev/") + 1);
    sprintf(full_path, "/dev/%s", name);

    free(namelist[0]);
    free(namelist);
    free(name);

    return full_path;
    // use <alphasort> in <scandir>
    // return a malloc'd name so doesn't corrupt.
    //unimplemented();
}

// return the most recently mounted ttyusb (the one
// mounted last).  use the modification time 
// returned by state.
char *find_ttyusb_last(void) {
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, filter, alphasort);
    if (n < 0) panic("scandir");
    if (n == 0) panic("no tty-usb devices found");
    struct stat st;
    char *last_device = strdupf(namelist[0]->d_name);
    time_t last_mod = 0;
    for (int i = 0; i < n; i++) {
        char *path = (char *) malloc(strlen(namelist[i]->d_name) + strlen("/dev/") + 1);
        sprintf(path, "/dev/%s", namelist[i]->d_name);
        stat(path, &st);
        free(path);

        if (st.st_mtime > last_mod) {
            last_mod = st.st_mtime;
            free(last_device);
            last_device = strdupf(namelist[i]->d_name);
            }
        free(namelist[i]);
        }
    free(namelist);
    char *full_path = (char *) malloc(strlen(last_device) + strlen("/dev/") + 1);
    sprintf(full_path, "/dev/%s", last_device);
    free(last_device);
    return full_path;
    //unimplemented();
}

// return the oldest mounted ttyusb (the one mounted
// "first") --- use the modification returned by
// stat()
char *find_ttyusb_first(void) {
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, filter, alphasort);
    if (n < 0) panic("scandir");
    if (n == 0) panic("no tty-usb devices found");
    struct stat st;
    char *first_device = strdupf(namelist[0]->d_name);
    time_t first_mod = 0x7FFFFFFF;
    for (int i = 0; i < n; i++) {
        char *path = (char *) malloc(strlen(namelist[i]->d_name) + strlen("/dev/") + 1);
        sprintf(path, "/dev/%s", namelist[i]->d_name);
        stat(path, &st);
        free(path);
        if (st.st_mtime < first_mod) {
            first_mod = st.st_mtime;
            free(first_device);
            first_device = strdupf(namelist[i]->d_name);
        }
        free(namelist[i]);
    }
    free(namelist);
    char *full_path = (char *) malloc(strlen(first_device) + strlen("/dev/") + 1);
    sprintf(full_path, "/dev/%s", first_device);
    free(first_device);
    return full_path;
    //unimplemented();
}
