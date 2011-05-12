/*
 * Copyright (C) 2011 Chien-An "Zero" Cho
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LOG_INFO "\033[m==> "
#define LOG_WARN "\033[1;33m==> "
#define LOG_SUCC "\033[1;32m==> "
#define LOG_FAIL "\033[1;31m==> "

void cleanup_log()
{
  printf("\033[m\n");
}

int help(char **argv)
{
  printf("Usage: %s [loader] [image]\n", argv[0]);
  return 0;
}

/* from usbboot.c (omap4boot) */
void *load_file(const char *file, unsigned *sz)
{
	void *data;
	struct stat s;
	int fd;
	
	fd = open(file, O_RDONLY);
	if (fd < 0)
		return 0;
	
	if (fstat(fd, &s))
		goto fail;
	
	data = malloc(s.st_size);
	if (!data)
		goto fail;
	
	if (read(fd, data, s.st_size) != s.st_size) {
		free(data);
		goto fail;
	}
	
	close(fd);
	*sz = s.st_size;
	return data;
	
fail:
	close(fd);
	return 0;
}

int main(int argc, char **argv)
{
  atexit(cleanup_log);
  
  printf("\033[1;32mPanda\033[mBoot - 0.1\n\n");
  
  if (argc == 1 || argc > 3)
    return help(argv);
  
  char *fnLoader = NULL, *fnImage = NULL;
  if (argc == 2)
  {
    printf(LOG_WARN "only found one arg, assume it's image\n");
    char dfLoader[10] = "aboot.bin";
    fnLoader = (char*)malloc(sizeof(dfLoader));
    strcpy(fnLoader, dfLoader);
    fnImage = argv[1];
  }
  
  printf(LOG_INFO "Loader: %s, Image: %s\n", fnLoader, fnImage);
  libusb_init(NULL);
  libusb_set_debug(NULL, 1);
  
  printf(LOG_INFO "waiting for \033[1;32mPanda\033[mBoard...\n");
  libusb_device_handle *hdev = NULL;
  while (hdev == NULL)
    hdev = libusb_open_device_with_vid_pid(NULL, 0x0451, 0xd00f);
  
  if (libusb_set_configuration(hdev, 1) != 0)
  {
    printf(LOG_FAIL "Unable to choose USB configuration\n");
    return -1;    
  }
  
  if (libusb_claim_interface(hdev, 0) != 0)
  {
    printf(LOG_FAIL "Unable to claim USB interface\n");
    return -1;    
  }
  
  printf(LOG_SUCC "USB device found, sending loader...\n");
  uint32_t msg_boot = 0xF0030002;
  uint32_t msg_size;
  void *data = load_file(fnLoader, &msg_size);
  if (data == NULL)
  {
    printf(LOG_FAIL "Unable to load Loader!\n");
    return -1;
  }
  
  printf(LOG_INFO "size: %d bytes\n", msg_size);

  int sz = 0;
  if (libusb_bulk_transfer(hdev, 0x01, (void*)&msg_boot, sizeof(msg_boot), &sz, 0) != 0)
  {
    printf(LOG_FAIL "USB transfer fail [boot]\n");
    return -1;
  }
  if (libusb_bulk_transfer(hdev, 0x01, (void*)&msg_size, sizeof(msg_size), &sz, 0) != 0)
  {
    printf(LOG_FAIL "USB transfer fail [size]\n");
    return -1;
  }
  if (libusb_bulk_transfer(hdev, 0x01, data, msg_size, &sz, 0) != 0)
  {
    printf(LOG_FAIL "USB transfer fail [data]\n");
    return -1;
  }

  printf(LOG_SUCC "Loader sent\n");
  printf(LOG_INFO "Wait for Loader ready...\n");
  
  usleep(100);
  
  if (libusb_bulk_transfer(hdev, 0x81, (void*)&msg_size, sizeof(msg_size), &sz, 0) != 0
      || msg_size != 0xaabbccdd)
  {
    printf(LOG_FAIL "Unrecognized Loader response (0x%X)\n", msg_size);
    return -1;
  }

  free(data);
  data = load_file(fnImage, &msg_size);
  if (data == NULL)
  {
    printf(LOG_FAIL "Unable to load image!\n");
    return -1;
  }

  printf(LOG_INFO "sending image, size: %d bytes\n", msg_size);
  if (libusb_bulk_transfer(hdev, 0x01, (void*)&msg_size, sizeof(msg_size), &sz, 0) != 0)
  {
    printf(LOG_FAIL "USB transfer fail [size]\n");
    return -1;
  }
  if (libusb_bulk_transfer(hdev, 0x01, data, msg_size, &sz, 0) != 0)
  {
    printf(LOG_FAIL "USB transfer fail [data]\n");
    return -1;
  }

  libusb_exit(NULL);
  return 0;
}