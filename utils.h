#ifndef _FIRMWARE_UTILS_H_
#define _FIRMWARE_UTILS_H_

#include "stdint.h"
#include "ip.h"

void puts(char *s);
void getline(char *buf, size_t len);

void print_mem(void *data, size_t len, char delim, int newline);
void print_mac(uint8_t *data);
void print_ipv4(uint8_t *data);
void print_ipv6(uint8_t *data);
void print_frame(frame_t *frame);

#endif
