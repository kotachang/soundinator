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

class Nimbler {
    private:
        const char* tag;

    public:
        const char* device_name;

        Nimbler(const char* device_name);
        ~Nimbler();
        
        void init();

};

#endif /* NIMBLER_HPP */