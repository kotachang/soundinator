#include "player.hpp"
#include "esp_sntp.h" // system time TODO: remove later

/*
-------------------------------------------------------------------------------------
Private Member Initialization
-------------------------------------------------------------------------------------
*/

/* FreeRTOS QueueHandle_t for I2S player */
QueueHandle_t Player::i2s_queue {0};
/* FreeRTOS TaskHandle_t for the main player task */
TaskHandle_t Player::player_task_handle {0};

/* I2S driver configuration */
const i2s_driver_config_t Player::i2s_config {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMP_RATE, // TODO: set this based on BLE input. might have to use i2s_set_sample_rates(i2s_port, rate)
    .bits_per_sample = static_cast<i2s_bits_per_sample_t>(AUDIO_BIT_LEN),
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = MAX_DMA_BUF_LEN,
    .use_apll = false
};

/* array that holds the audio to be played */
int16_t Player::audio[AUDIO_ARR_SZ] {0};

/*
-------------------------------------------------------------------------------------
Public Functions
-------------------------------------------------------------------------------------
*/

/* Constructor */
Player::Player() {
}

/* Deconstructor */
Player::~Player() {
}

/* Initialize I2S Driver */
void Player::init() {
    // TODO: fix comment style
    // TOD): add proper comments
    
    i2s_driver_install(i2s_port, (i2s_config_t *)&i2s_config, I2S_EVT_Q_LEN, &i2s_queue); // install and start i2s driver
    i2s_set_pin(i2s_port, &i2s_pins); // set up the gpio pins for i2s
    i2s_zero_dma_buffer(i2s_port); // clear the DMA buffers
    
    /* Start the main player task */
    // TODO: check stack size
    xTaskCreate(_player_task, "I2S Player Task", PLAYER_TASK_STACK_SZ, nullptr, PLAYER_TASK_PRIORITY, &player_task_handle);
    //TODO: i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver
}

/* Play audio TODO: implement communication with BLE task */
int Player::play() {
    // TODO: check if player task already exists
    return 0;
}


/*
-------------------------------------------------------------------------------------
Private Functions
-------------------------------------------------------------------------------------
*/

/* Main player task */
void Player::_player_task(void *param) {
    size_t bytes_written {0};
    i2s_event_t event;
    int16_t bytes_left {0};
    
    while (true) {
        if (xQueueReceive(i2s_queue, &event, portMAX_DELAY) == pdPASS) {
            if (event.type == I2S_EVENT_TX_DONE) {
                // TODO: handle end of audio. right now it loops forever
                while (true) {
                    _get_next_chunk();
                    bytes_left = AUDIO_ARR_SZ*sizeof(int16_t);
                    bytes_written = 0;
                    while (bytes_left) {
                        /* Send audio */
                        i2s_write(I2S_NUM_0, &audio + bytes_written, AUDIO_ARR_SZ*sizeof(int16_t) - bytes_written, &bytes_written, portMAX_DELAY);
                        bytes_left -= bytes_written;
                    }
                }
            }
        }
    }
}

void Player::_get_next_chunk(void) {
#ifdef CONFIG_TEST_NOTE
    fill_c_note_array(audio, AUDIO_ARR_SZ);
#else /* CONFIG_TEST_NOTE */
    // TODO: grab audio from BLE
#endif /* CONFIG_TEST_NOTE */
}