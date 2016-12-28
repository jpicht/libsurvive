#ifndef DIS_H
#define DIS_H

#include <stdint.h>

typedef enum {
	STATE_INVALID = 0,
	STATE_LOCKED = 1,
} disambiguator_state;

struct disambiguator {
	disambiguator_state locked;
	uint8_t last_type;
	uint32_t last_timestamp;
};

/**
 * classification result
 */
typedef enum {
	P_UNKNOWN = 0,
	P_MASTER  = 1,
	P_SLAVE   = 2,
	P_SWEEP   = 3,
	P_SPURIOS = 4,
	P_MISSING = 5,
	P_BACKWRD = 6,
} pulse_type;

/**
 * Initialize the struct (will only memset(d, 0))
 */
void disambiguator_init(struct disambiguator * d);

/**
 * Classify one pulse
 * @param time   Timestamp of rising edge in ticks
 * @param length Length of pulse in ticks
 * @return type of pulse
 */
pulse_type disambiguator_step(struct disambiguator * d, uint32_t time, uint32_t length);

#endif /* DIS_H */