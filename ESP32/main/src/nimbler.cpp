#include <stdio.h>
#include <string>
#include "nimbler.hpp"

/*
-------------------------------------------------------------------------------------
Static Private Member Initialization
-------------------------------------------------------------------------------------
*/

uint8_t Nimbler::ble_addr_type = 0;
uint16_t Nimbler::conn_hndl = 0;
uint16_t Nimbler::attr_hndl_audio = 0;
uint8_t Nimbler::notify_chr_audio = 0;

/* UUIDs for main ble service and characteristics */
const ble_uuid16_t Nimbler::service_uuid = {
    .u = BLE_UUID_TYPE_16,
    .value = service_uuid_val
};
const ble_uuid16_t Nimbler::audio_info_uuid = {
    .u = BLE_UUID_TYPE_16,
    .value = audio_info_uuid_val
};
const ble_uuid16_t Nimbler::receive_audio_uuid = {
    .u = BLE_UUID_TYPE_16,
    .value = receive_audio_uuid_val
};

/* GATT characteristic structure definition */
const struct ble_gatt_chr_def Nimbler::gatt_chars[N_CHARACTERISTICS] = {
    {
        .uuid = (ble_uuid_t *)&audio_info_uuid,
        .access_cb = _audio_info,
        .flags = BLE_GATT_CHR_F_READ
    },
    {
        .uuid = (ble_uuid_t *)&receive_audio_uuid,
        .access_cb = _receive_audio,
        .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY,
        .val_handle = &attr_hndl_audio
    },
    {
        0, // no more characteristics
    },
};

/* GATT service structure definition */
const struct ble_gatt_svc_def Nimbler::gatt_svcs[N_SERVICES] = {
    {   .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t *)&service_uuid,                 // Define UUID for device type
        .characteristics = gatt_chars,
    },
    {
        0, // no more services
    },
};

/*
-------------------------------------------------------------------------------------
Public Functions
-------------------------------------------------------------------------------------
*/

/* Constructor */
Nimbler::Nimbler(const char* device_name) {
    this->device_name = device_name;
    ble_addr_type = 0; // determines the best address type automatically
}

/* Deconstructor */
Nimbler::~Nimbler() {
}

/* Initialize the Nimble stack and call init() functions for GAP, GATT 
* Follows programming sequence from https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-reference/bluetooth/nimble/index.html
* as well as the bleprph example by espressif
*/
void Nimbler::init() {
    /* Initialize the NimBLE protocol stack */
    int ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.sync_cb = _ble_app_on_sync;   // initialize advertisement only after the host & controller are synced
    ble_hs_cfg.reset_cb = _ble_app_on_reset;  // if NimBLE stack resets after crash, print reason
    
    /* Initialize GAP, GATT services */
    ble_svc_gap_init();     // General Access Profile
    ble_svc_gatt_init();    // General Attribute Profile
    ret = ble_gatts_count_cfg(gatt_svcs);
    assert(ret == 0);
    ret = ble_gatts_add_svcs(gatt_svcs);
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
    nimble_port_run(); // this function will return only when nimble_port_stop() is executed
    /* After the Nimble port runs, the ble advertisement starts with the _ble_app_on_sync function as a callback */
    nimble_port_freertos_deinit();
}

/* Start of BLE application after NimBLE host & controller are synced */
void Nimbler::_ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // determines the best address type automatically
    _ble_app_advertise();                     // define the BLE connection
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
    device_name = ble_svc_gap_device_name(); // read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    /* GAP - device connectivity definition */
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // undirected & connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // general & discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, _ble_gap_event, NULL);
}

/* BLE GAP event (connection) handling */
int Nimbler::_ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        /* If connect event status is not 0, then advertise again */
        if (event->connect.status != 0)
        {
            _ble_app_advertise();
        }
        conn_hndl = event->connect.conn_handle;
        break;
    /* Advertisement duration over. Start advertising again */
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(tag, "BLE GAP EVENT ADV COMPLETE");
        _ble_app_advertise();
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(tag, "BLE GAP EVENT DISCONNECT. Reason: %d", event->disconnect.reason);
        _ble_app_advertise();
        break;
    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(tag, "BLE GAP EVENT SUBSCRIBE. conn: %d, attr: %d, reason:%d", event->subscribe.conn_handle, event->subscribe.attr_handle, event->subscribe.reason);
        if (event->subscribe.attr_handle == attr_hndl_audio) {
            if (event->subscribe.reason == BLE_GAP_SUBSCRIBE_REASON_WRITE) {
                ESP_LOGI(tag, "SUBSCRIBED TO RECEIVE AUDIO");
                notify_chr_audio = 1;
            } else if (event->subscribe.reason == BLE_GAP_SUBSCRIBE_REASON_TERM) {
                ESP_LOGI(tag, "UNSUBSCRIBED TO RECEIVE AUDIO");
                notify_chr_audio = 0;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

/* Receive audio from client */
int Nimbler::_receive_audio(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    ESP_LOGI(tag, "Receive audio callback. conn:%d attr:%d", conn_handle, attr_handle);
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

/* Receive incoming audio info from client */
int Nimbler::_audio_info(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{   
    uint8_t data[1];
    /* TODO: check if audio can be played and set data[] accordingly */
    data[0] = 1;
    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, sizeof(data));
    if (notify_chr_audio) {
        ble_gatts_notify_custom(con_handle, attr_handle, om);
    }
    return 0;
}