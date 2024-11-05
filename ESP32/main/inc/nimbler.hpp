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

#define N_SERVICES              (2)
#define N_CHARACTERISTICS       (3)
#define service_uuid_val        (0xBEEF)
#define device_read_uuid_val    (0xBEBE)
#define device_write_val        (0xFACE)

class Nimbler {
    public:
        Nimbler(const char* device_name);
        ~Nimbler();
        
        void init();

    private:
        static constexpr char* tag = "Nimbler"; // tag used for logs
        const char* device_name;
        
        static uint8_t ble_addr_type; // bluetooth address type
        static uint16_t conn_hndl; // bluetooth connection handle
        static uint16_t attr_hndl_audio; // attribute handle for audio characteristic

        static const ble_uuid16_t service_uuid; // uuid for the main ble service
        static const ble_uuid16_t device_read_uuid; // uuid for read characteristic
        static const ble_uuid16_t device_write_uuid; // uuid for write characteristic

        static const struct ble_gatt_chr_def gatt_chars[N_CHARACTERISTICS];
        static const struct ble_gatt_svc_def gatt_svcs[N_SERVICES];

        static void _ble_task(void *param);
        static void _ble_app_on_sync();
        static void _ble_app_on_reset(int reason);
        static void _ble_app_advertise();
        static int _ble_gap_event(struct ble_gap_event *event, void *arg);
        static int _device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
        static int _device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
};

#endif /* NIMBLER_HPP */