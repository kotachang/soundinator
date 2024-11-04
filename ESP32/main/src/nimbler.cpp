#include <stdio.h>
#include <string>
#include "nimbler.hpp"

/* Initialize private static members */
uint8_t Nimbler::ble_addr_type = 0;
uint16_t Nimbler::attr_hndl_audio = 0;

/* Constructor */
Nimbler::Nimbler(const char* device_name) {
    this->device_name = device_name;
    ble_addr_type = 0; // Determines the best address type automatically
}

/* Deconstructor */
Nimbler::~Nimbler() {
}

/* Initialize the Nimble stack and call init() functions for GAP, GATT */
void Nimbler::init() {
    /* Initialize the NimBLE protocol stack */
    int ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.sync_cb = _ble_app_on_sync;   // Initialize advertisement only after the host & controller are synced
    ble_hs_cfg.reset_cb = _ble_app_on_reset;  // If NimBLE stack resets after crash, print reason
    
    /* Initialize GAP, GATT services */
    ble_svc_gap_init();     // General Access Profile
    ble_svc_gatt_init();    // General Attribute Profile
    ret = ble_gatts_count_cfg(this->gatt_svcs);
    assert(ret == 0);
    ret = ble_gatts_add_svcs(this->gatt_svcs);
    assert(ret == 0);

    /* Name BLE Device */
    ble_svc_gap_device_name_set(this->device_name);
    
    /* Run BLE in a new FreeRTOS task */
    nimble_port_freertos_init(_ble_task);
}

/*
-------------------------------------------------------------------------------------
Private Functions
-------------------------------------------------------------------------------------
*/

/* The (almost) infinite FreeRTOS task */
void Nimbler::_ble_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
    /* After the Nimble port runs, the ble advertisement starts with the _ble_app_on_sync function as a callback */
    nimble_port_freertos_deinit();
}

/* Start of BLE application after NimBLE host & controller are synced */
void Nimbler::_ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    _ble_app_advertise();                     // Define the BLE connection
}

/* Callback after NimBLE stack crashes and resets */
void Nimbler::_ble_app_on_reset(int reason)
{
    ESP_LOGE(tag, "Resetting state; reason=%d\n", reason);
}

/* Define the BLE connection and start advertising */
void Nimbler::_ble_app_advertise(void)
{
    /* GAP - set fields for device name */
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    /* GAP - device connectivity definition */
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // undirected & connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // general & discoverable
    //ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// Write data to ESP32 defined as server
int Nimbler::_device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ESP_LOGI("", "device write. conn:%d attr:%d", conn_handle, attr_handle);
    //ESP_LOGI("", "device write. subscribe handle:%d", this->attr_hndl_audio);
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

// Read data from ESP32 defined as server
int Nimbler::_device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}