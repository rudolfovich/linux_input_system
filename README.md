linux_input_system
============

Object oriented linux input system

Installation
============
    mkdir build/
    cd build
    ../configure --prefix=/usr/local
    make && sudo make install && sudo ../post_install.sh
    make distclean

Features
============
 - read input events from keyboard, mouse, joystick, gamepad and wheel(untested);
 - safety remove device from stack after it was plugged out;

Issues
============
 - auto connect for runtime plugged devices does not work;

TODO
============
 - examples;
 - docxygen;
 - automaticly add connected device to stack;

