#ifdef CONFIG_TEST_NOTE
#include <cstdint>
#define HALF_PERIOD_MID_C (42) // num of samples in half period of a piano middle C (261.63 Hz) for a 22050 Hz sampling rate
#define AMPLITUDE (32768/5) // audio amplitude. 1/5 of max int16_t seems to be max when it comes out the speaker
#define NOTE_LEN (512) // len of note[] array

void fill_c_note_array(int16_t *note, int len);

#endif /* CONFIG_TEST_NOTE */