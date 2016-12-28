#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dis.h"

void bits_to_buffer(char * buffer, uint8_t t) {
	buffer[0] = t & 4 ? '1' : '0';
	buffer[1] = t & 2 ? '1' : '0';
	buffer[2] = t & 1 ? '1' : '0';
	buffer[4] = 0;
}

int main() {
	FILE * f = fopen( "new_lightcap_data.csv", "r" );
	if (f == NULL) {
		fprintf(stderr, "ERROR OPENING INPUT FILE\n");
		return -1;
	}

	char bitbuffer[16];

	struct disambiguator d;
	disambiguator_init(&d);
	for (;;) {
		char controller[10];
		int sensor;
		int unknown;
		uint32_t length;
		uint32_t time;

		if (fscanf(f, "%s %d %d %d %d", controller, &sensor, &unknown, &length, &time) != 5) {
			break;
		}
		if (strcmp(controller, "HMD") != 0) continue;

		char cc = (length - 2750) / 500;
		bits_to_buffer(bitbuffer, cc);
		char * format = "%s%-5s %s %2d %10d %10d %10d %5d %3s\n";
		switch (disambiguator_step(&d, time, length)) {
			default:
			case P_UNKNOWN:
				printf(format, "\e[41m\e[97m", "UNKN", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, bitbuffer);
				continue;
			case P_MASTER:
				printf(format, "\e[92m", "MASTR", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, bitbuffer);
				continue;
			case P_SLAVE:
				printf(format, "\e[32m", "SLAVE", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, bitbuffer);
				continue;
			case P_SWEEP:
				printf(format, "\e[33m", "SWEEP", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, "");
				continue;
			case P_SPURIOS:
				printf(format, "\e[41m\e[97m", "SPUR", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, bitbuffer);
				continue;
			case P_MISSING:
				printf(format, "\e[41m\e[97m", "MISS", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, bitbuffer);
				continue;
			case P_BACKWRD:
				printf(format, "\e[41m\e[97m", "BACK", controller, sensor, d.last_timestamp, time, time - d.last_timestamp, length, bitbuffer);
				continue;
		}
	}
	fclose(f);
}
