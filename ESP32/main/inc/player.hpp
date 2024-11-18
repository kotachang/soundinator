#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "sdkconfig.h"
#include "driver/i2s.h"
#include "freertos/queue.h"
#include "esp_mac.h"
#include "esp_log.h"
#ifdef CONFIG_TEST_NOTE
#include "test_note.hpp"
#endif /* CONFIG_TEST_NOTE */

#define INT16_MAX (32768)
#define SAMP_RATE (22050)
#define AUDIO_BIT_LEN (16)
#define AUDIO_BYTES (AUDIO_BIT_LEN/8)
#define AUDIO_ARR_SZ (MAX_DMA_BUF_LEN/AUDIO_BYTES)
#define MAX_DMA_BUF_LEN (1024)
#define DMA_BUF_COUNT (4)
#define I2S_EVT_Q_LEN (4)

#define PLAYER_TASK_PRIORITY (1)
#define PLAYER_TASK_STACK_SZ (4096*2)

class Player
{
public:
    Player();
    ~Player();

    void init();
    int play();

private:
    i2s_port_t i2s_port {I2S_NUM_0};
    static const i2s_driver_config_t i2s_config;
    static QueueHandle_t i2s_queue;

    /* I2S pins */
    i2s_pin_config_t i2s_pins {
        .bck_io_num = GPIO_NUM_27,      // BCLK on MAX98357A
        .ws_io_num = GPIO_NUM_14,       // LRS on MAX98357A
        .data_out_num = GPIO_NUM_26,    // DIN on MAX98357A
        .data_in_num = -1};             // unused

    static int16_t audio[AUDIO_ARR_SZ];

    static TaskHandle_t player_task_handle;
    static void _player_task(void *param);

    static void _get_next_chunk(void);
};

#endif /* PLAYER_HPP */