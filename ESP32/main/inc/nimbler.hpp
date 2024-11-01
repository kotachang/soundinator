#ifndef NIMBLER_HPP
#define NIMBLER_HPP

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

#define N_SERVICES (2)
#define N_CHARACTERISTICS (3)

class Nimbler {
    public:
        const char* device_name;

        Nimbler(const char* device_name);
        ~Nimbler();
        
        void init();

        /* Callbacks need to be public for NimBLE */
        static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
        static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

    private:
        // tag for logs
        const char* tag = "Nimbler";

        // attribute handle for audio characteristic
        uint16_t attr_hndl_audio;

        // uuids for main service and characteristics
        uint16_t service_uuid_val = 0xBEEF;
        const ble_uuid16_t service_uuid = {
            .u = BLE_UUID_TYPE_16,
            .value = service_uuid_val
        };
        uint16_t device_read_uuid_val = 0xBEBE;
        const ble_uuid16_t device_read_uuid = {
            .u = BLE_UUID_TYPE_16,
            .value = device_read_uuid_val
        };
        uint16_t device_write_val = 0xFACE;
        const ble_uuid16_t device_write_uuid = {
            .u = BLE_UUID_TYPE_16,
            .value = device_write_val
        };

        // gatt characteristic structure definition
        const struct ble_gatt_chr_def gatt_chars[N_CHARACTERISTICS] = {
            {
                .uuid = (ble_uuid_t *)&device_read_uuid,
                .access_cb = device_read,
                .flags = BLE_GATT_CHR_F_READ
            },
            {
                .uuid = (ble_uuid_t *)&device_write_uuid,
                .access_cb = device_write,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &attr_hndl_audio
            },
            {
                0, /* No more characteristics*/
            },
        };
        // gatt service structure definition
        const struct ble_gatt_svc_def gatt_svcs[N_SERVICES] = {
            {   .type = BLE_GATT_SVC_TYPE_PRIMARY,
                .uuid = (ble_uuid_t *)&service_uuid,                 // Define UUID for device type
                .characteristics = gatt_chars,
            },
            {
                0, /* No more services */
            },
        };

};

#endif /* NIMBLER_HPP */