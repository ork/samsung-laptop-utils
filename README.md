samsung-laptop-utils
====================

Small desktop utilities to control a Samsung Laptop.

Usage
-----

### Dependencies

* glib-2.0
* gio-2.0
* libnotify

### Building

    make

Don't forget to run `chown root $BIN ; chmod u+s $BIN` after building,
as `setuid(3)` is needed for privileged operations.

