/* 
* Basic structure of the code was adopted from https://github.com/SIMS-IOT-Devices/FreeRTOS-ESP-IDF-BLE-Server/blob/main/proj3.c and the NimBLE tutorials
*/

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

char *tag = "BLE-Server";
uint8_t ble_addr_type;
void ble_app_advertise(void);

// Write data to ESP32 defined as server
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

// Read data from ESP32 defined as server
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}

// Array of pointers to other service definitions
// UUID - Universal Unique Identifier
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0xBEEF),                 // Define UUID for device type
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0xBEBE),           // Define UUID for reading
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},
         {.uuid = BLE_UUID16_DECLARE(0xFACE),           // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0}};

// BLE GAP event (connection) handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        /* If connect event status is not 0, then advertise again */
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    /* Advertisement duration over. Start advertising again */
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT ADV COMPLETE");
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECT. Reason: %d", event->disconnect.reason);
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

// Define the BLE connection and start advertising
void ble_app_advertise(void)
{
    // GAP - set fields for device name
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // undirected & connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // general & discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// Start of BLE application
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

// The (almost) infinite FreeRTOS task
void host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed

    nimble_port_freertos_deinit();
}

/*
* Follow programming sequence from https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/bluetooth/nimble/index.html
* as well as the bleprph example by espressif
*/
void app_main()
{
    /* Initialize Non Volatile Storage — it is used to store physical layer calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize the NimBLE protocol stack */
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.sync_cb = ble_app_on_sync;   // Initialize advertisement only after the host & controller are synced
    ble_hs_cfg.reset_cb = ble_app_on_sync;  // If NimBLE stack resets after crash, start advertisement process again
    /* TODO: if implemeting persistent connection then add
    * ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    * and look into 'store' 
    */
    /* TODO: if implementing Security Manager features, set the host settings here
    * ex) secure connection: ble_hs_cfg.sm_sc = 1;
    * Note: if remote device doesn't have secure connection abilities connection fails
    */

    /* Initialize GAP, GATT, ANS services */
    int rc;
    ble_svc_gap_init();     // General Access Profile
    ble_svc_gatt_init();    // General Attribute Profile
    ble_svc_ans_init();     // Alert and Notifications
    rc = ble_gatts_count_cfg(gatt_svcs);
    assert(rc == 0);
    rc = ble_gatts_add_svcs(gatt_svcs);
    assert(rc == 0); 

    /* Name BLE Device */
    ble_svc_gap_device_name_set("Soundinator");
    
    /* Run BLE in a new FreeRTOS task */
    nimble_port_freertos_init(host_task);
}