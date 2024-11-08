#include "player.hpp"

/*
-------------------------------------------------------------------------------------
Private Member Initialization
-------------------------------------------------------------------------------------
*/
const i2s_driver_config_t Player::i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100, // TODO: set this based on BLE input
    .bits_per_sample = (i2s_bits_per_sample_t)16, // TODO: set this based on BLE input
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 4,
    .dma_buf_len = 1024, // TODO: include reasoning
    .use_apll = false
};

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
    //install and start i2s driver
    i2s_driver_install(i2s_port, (i2s_config_t *)&i2s_config, 4, &i2s_queue);
    // set up the i2s pins
    i2s_set_pin(i2s_port, &i2s_pins);
    // clear the DMA buffers
    i2s_zero_dma_buffer(i2s_port);
    i2s_set_sample_rates(i2s_port, 22050); //set sample rates

    xTaskCreate(_i2s_player, "I2S Player Task", 4096, this, 1, &player_task_handle);
    //TODO: i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver
}


/*
-------------------------------------------------------------------------------------
Private Functions
-------------------------------------------------------------------------------------
*/

/* Main player task */
void Player::_i2s_player(void *param) {
    size_t len = 100; // in bytes
    uint8_t bytes[len] = {0};
    size_t i2s_bytes_written;
    i2s_write(I2S_NUM_0, &bytes, len, &i2s_bytes_written, portMAX_DELAY);
}