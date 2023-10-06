#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

void print_num(int64_t x) {
	bool neg = false;
	if (x < 0) {
		neg = true;
		x *= -1;
	}
	char buf[32];
	size_t buf_sz = 1;
	buf[sizeof(buf) - buf_sz] = '\n';
	do {
		buf[sizeof(buf) - buf_sz - 1] = x % 10 + '0';
		buf_sz++;
		x /= 10;
	} while(x);
	if (neg) {
		buf[sizeof(buf) - buf_sz - 1] = '-';
		buf_sz++;
	}
	write(1, &buf[sizeof(buf) - buf_sz], buf_sz);
}

