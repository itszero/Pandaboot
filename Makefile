all: pandaboot

pandaboot: pandaboot.c
	gcc `pkg-config --libs --cflags libusb-1.0` -o pandaboot pandaboot.c
  