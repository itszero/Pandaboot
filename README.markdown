Pandaboot
=========

Introduction
------------

Pandaboot is a USB bootstrapping program designed for Pandaboard. It heavily depends on the [omap4boot](https://github.com/swetland/omap4boot "Homepage of omap4boot"). Pandaboot implements the TI OMAP USB booting protocol in libusb, instead of Linux USBDevFS used in omap4boot. This allows Pandaboot running on more UNIX-variant systems like Mac OSX and *BSD series OS. Windows is not tested, but however should work.

Usage
-----

* Connects your Pandaboard by USB mini head.
* run ./pandaboot uboot.bin to load and run uboot on board
* or use ./pandaboot loader.bin image.bin to specify your own loader and image file

Note: A sample aboot.bin and uboot.bin has been provided for your reference.

Install
-------

Tested on OSX 10.6

* brew install libusb
* make
* you should get pandaboot in the directory

Terminology
-----------

Loader is a program which transferred via TI OMAP USB booting protocol. It handles basic USB functionality and allows pandaboot to do the image transferring to board's RAM.

Image is the file you wish to transfer to the loader, it usually can be a full-featured bootloader or even an OS kernel.

License
-------

    Copyright (C) 2011 Chien-An "Zero" Cho
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in
        the documentation and/or other materials provided with the 
        distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
