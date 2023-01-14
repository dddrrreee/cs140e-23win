# Software Installation

## Drivers

### macOS

- Download and install the drivers for a
  CP210x USB-to-UART driver as described in the
  [cs107e docs](http://cs107e.github.io/guides/install/mac/).
- Make sure you reboot after doing so! (Go Apple!)

### Linux

- You shouldn't need drivers, however you may need to add yourself to
  the `dialout` group (or `serial`) depending on the distribution.

      sudo adduser <your username> dialout

  If you do this, make sure you login and logout.

  If that still doesn't work, you may have to remove `modemmanager`:

      sudo apt-get remove modemmanager

  In this case you may need to reboot.

## Toolchain

### macOS

Use the [cs107e install notes](http://cs107e.github.io/guides/install/mac/).
Note: do not install the python stuff. We will use their custom brew formula!

### Linux

For [ubuntu/linux](https://askubuntu.com/questions/1243252/how-to-install-arm-none-eabi-gdb-on-ubuntu-20-04-lts-focal-fossa), ARM recently
changed their method for distributing the tool change. Now you
must manually install. As of this lab, the following works:

        wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2

        sudo tar xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /usr/opt/

We want to get these binaries on our `$PATH` so we don't have to type the
full path to them every time. There's a fast and messy option, or a slower
and cleaner option.

The fast and messy option is to add symlinks to these in your system `bin`
folder:

        sudo ln -s /usr/opt/gcc-arm-none-eabi-10.3-2021.10/bin/* /usr/bin/

The cleaner option is to add `/usr/opt/gcc-arm-none-eabi-10.3-2021.10/bin` to
your `$PATH` variable in your shell configuration file (e.g., `.zshrc` or
`.bashrc`), save it, and `source` the configuration. When you run:

        arm-none-eabi-gcc
        arm-none-eabi-ar
        arm-none-eabi-objdump

You should not get a "Command not found" error.

You may also have to add your username to the `dialout` group.

If gcc can't find header files, try:

       sudo apt-get install libnewlib-arm-none-eabi
