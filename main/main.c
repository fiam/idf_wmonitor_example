/* ESP WMonitor Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_event_loop.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"

#include <idf_wmonitor/idf_wmonitor.h>

/*set the ssid and password via "make menuconfig"*/
#define DEFAULT_SSID CONFIG_WIFI_SSID
#define DEFAULT_PASSWORD CONFIG_WIFI_PASSWORD

static const char *TAG = "WMONITOR-EXAMPLE";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return idf_wmonitor_event_handler(ctx, event);
}

static void periodic_log_task(void *arg)
{
    int ii = 0;
    for (;;)
    {
        ESP_LOGI(TAG, "Hello %d", ii++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
#ifdef CRASH_INTENTIONALLY
        if (++ii == 30)
        {
            int *a = NULL;
            *a = 42;
        }
#endif
    }
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    idf_wmonitor_config_t config = IDF_WMONITOR_CONFIG_DEFAULT_WITH_CREDENTIALS(DEFAULT_SSID, DEFAULT_PASSWORD);
    idf_wmonitor_opts_t opts = {
        .config = config,
        .flags = IDF_WMONITOR_WAIT_FOR_CLIENT_IF_COREDUMP,
    };
    ESP_ERROR_CHECK(idf_wmonitor_start(&opts));

    xTaskCreate(periodic_log_task, "PERIODIC-LOG", 4096, NULL, tskIDLE_PRIORITY, NULL);
}