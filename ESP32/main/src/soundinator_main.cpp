#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "nimbler.hpp"

const char* tag = "Main"; // tag for logs

/* Main function */
extern "C" void app_main()
{
    /* Initialize Non Volatile Storage — it is used to store physical layer calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Start the BLE task with Nimbler */
    Nimbler nimbler("Soundinator");
    nimbler.init();
    ESP_LOGI(tag, "Nimbler initialized");
}