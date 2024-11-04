#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/ans/ble_svc_ans.h"
#include "sdkconfig.h"
#include "nimbler.hpp"

// tag for logs
const char* tag = "Main";

/*
* Follow programming sequence from https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/bluetooth/nimble/index.html
* as well as the bleprph example by espressif
*/
extern "C" void app_main()
{
    /* Initialize Non Volatile Storage — it is used to store physical layer calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Create a nimbler object and call init() which configures BLE and starts the BLE task
    * Note: FreeRTOS in ESP-IDF should NOT call vTaskStartScheduler() and vTaskEndScheduler()
    */
    Nimbler nimbler("Soundinator");
    nimbler.init();
    ESP_LOGI(tag, "main task called nimbler");
}