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

class Player {
    public:
        Player();
        ~Player();

        void init();

    private:
        i2s_port_t i2s_port;
        static const i2s_driver_config_t i2s_config;
        QueueHandle_t i2s_queue;
        
        /* I2S pins */
        i2s_pin_config_t i2s_pins = {
            .bck_io_num = 26,
            .ws_io_num = 25,
            .data_out_num = 22,
            .data_in_num = -1
        };

        static TaskHandle_t player_task_handle;
        static void _i2s_player(void *param);

};

#endif /* PLAYER_HPP */