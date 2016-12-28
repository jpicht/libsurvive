#include "dis.h"
#include <string.h>
#include <stdbool.h>

//       ID  SKIP DATA AXIS  WO/D
// 3000:  0     0    0    0     0
// 3500:  1     0    0    1     1
// 4000:  2     0    1    0     0
// 4500:  3     0    1    1     1
// 5000:  4     1    0    0     2
// 5500:  5     1    0    1     3
// 6000:  6     1    1    0     2
// 6500:  7     1    1    1     3

pulse_type pulse_type_without_data[] = {
	0, 1, 0, 1,
	2, 3, 2, 3,
};

pulse_type next_pulse_type_without_data[] = {
	2, 3, 0, 1,
};

#define PULSE_WITHOUT_DATA(T) (((T>>1)%2)|(T&1))
#define NEXT_PULSE_TYPE_MASTR(T) ((T+1)%2)
#define NEXT_PULSE_TYPE_SLAVE(T) ((T+2)%4)

#define IS_VALID_NEXT_SLAVE(L, C) (((L&1) == (C&1)) && ((L&C&4) == 0))
#define IS_VALID_NEXT_MASTR(L, C) ((L&1) != (C&1))

inline void disambiguator_init(struct disambiguator * d) {
	memset(d, 0, sizeof(struct disambiguator));
}

pulse_type calculate_pulse_type(uint32_t length) {
	return (length - 2750) / 500;
}

#define IS_POSSIBLE_SYNC(L) (L > 2750 && L < 6750)
#define NOT_A_POSSIBLE_SYNC(L) (L < 2750 || L > 6750)

pulse_type disambiguator_step(struct disambiguator * d, uint32_t time, uint32_t length) {
	if (d->locked == STATE_LOCKED) {
		int32_t diff = time - d->last_timestamp;

		if (diff < 0 && diff > -400000) {
			return P_BACKWRD;
		}

		// more incarnations of the current sync pulse?
		if (diff < 1000) {
			if (IS_POSSIBLE_SYNC(length) && calculate_pulse_type(length) != d->last_type) return P_SPURIOS;

			d->last_timestamp = (d->last_timestamp*3/4) + (time/4);

			return P_MASTER;
		}

		// too soon
		if (diff < 18500) {
			return P_SPURIOS;
		}

		// possible slave sync pulse

		if (diff < 21500) {
			if (NOT_A_POSSIBLE_SYNC(length)) return P_SPURIOS;

			pulse_type t = calculate_pulse_type(length);
			if (IS_VALID_NEXT_SLAVE(d->last_type, t)) {
				return P_SLAVE;
			}

			return P_SPURIOS;
		}
		
		// too early for sweep

		if (diff < 70000) {
			return P_SPURIOS;
		}

		// we could encounter sync pulse

		if (diff < 350000) {
			return P_SWEEP;
		}

		// too late for sweep

		if (NOT_A_POSSIBLE_SYNC(length)) return P_SPURIOS;

		pulse_type t = calculate_pulse_type(length);
		if (IS_VALID_NEXT_MASTR(d->last_type, t)) {
			d->last_timestamp = time;
			d->last_type = t;
			return P_MASTER;
		}
		
		if (diff > 415000) {
			d->last_timestamp += 1;
			d->last_type = NEXT_PULSE_TYPE_MASTR(t);
			return P_MISSING;
		}

		return P_SPURIOS;
	} else {
		if (NOT_A_POSSIBLE_SYNC(length)) return P_UNKNOWN;

		d->last_timestamp = time;
		d->last_type = calculate_pulse_type(length);
		d->locked = STATE_LOCKED;
		return P_MASTER;
	}

	return P_UNKNOWN;
}