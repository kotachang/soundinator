#include <stdio.h>
#include <string>
#include "nimbler.hpp"


const char* tag = "Nimbler";

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
    ble_svc_ans_init();     // Alert and Notifications
    //ret = ble_gatts_count_cfg(gatt_svcs);
    assert(ret == 0);
    //ret = ble_gatts_add_svcs(gatt_svcs);
    assert(ret == 0);

    /* Name BLE Device */
    ble_svc_gap_device_name_set(this->device_name);
    
    /* Run BLE in a new FreeRTOS task */
    //nimble_port_freertos_init(host_task);
}