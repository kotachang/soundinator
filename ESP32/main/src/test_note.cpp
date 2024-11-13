#ifdef CONFIG_TEST_NOTE
#include "test_note.hpp"

/* Fill in the input array with a square wave to play the middle C on a piano */
void fill_c_note_array(int16_t *note, int len) {
    static int64_t count = 0;
    static int8_t val = AMPLITUDE;

    /* Create a square wave of frequency (1/HALF_PERIOD_MID_C*2) */
    for (int i=0; i<len; i++) {
        if (count % (HALF_PERIOD_MID_C*2) == 0) { // invert val every period (half for left, half for right channel)
            val = -val;
        }
        note[i] = val;
        count++;
    }
}

#endif