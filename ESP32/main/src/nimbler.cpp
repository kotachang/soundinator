#include <stdio.h>
#include <string>
#include "nimbler.hpp"


/* Constructor */
Nimbler::Nimbler(const char* device_name) {
    this->device_name = device_name;
}

/* Deconstructor */
Nimbler::~Nimbler() {
}

void Nimbler::init() {
    /* Initialize the NimBLE protocol stack */
    int ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    //ble_hs_cfg.sync_cb = ble_app_on_sync;   // Initialize advertisement only after the host & controller are synced
    //ble_hs_cfg.reset_cb = ble_app_on_sync;  // If NimBLE stack resets after crash, start advertisement process again
    
    /* Initialize GAP, GATT, ANS services */
    ble_svc_gap_init();     // General Access Profile
    ble_svc_gatt_init();    // General Attribute Profile
    ret = ble_gatts_count_cfg(this->gatt_svcs);
    assert(ret == 0);
    ret = ble_gatts_add_svcs(this->gatt_svcs);
    assert(ret == 0);

    /* Name BLE Device */
    ble_svc_gap_device_name_set(this->device_name);
    
    /* Run BLE in a new FreeRTOS task */
    //nimble_port_freertos_init(host_task);
}


// Write data to ESP32 defined as server
int Nimbler::device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ESP_LOGI("", "device write. conn:%d attr:%d", conn_handle, attr_handle);
    //ESP_LOGI("", "device write. subscribe handle:%d", this->attr_hndl_audio);
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

// Read data from ESP32 defined as server
int Nimbler::device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}