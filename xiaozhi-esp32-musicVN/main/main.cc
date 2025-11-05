#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <esp_system.h>
#include <esp_ota_ops.h>
#include <lwip/sockets.h>
#include "sdkconfig.h"

#include "application.h"
#include "system_info.h"

#define TAG "main"
#define OTA_TAG "OTA"
#define OTA_PORT CONFIG_WIFI_OTA_PORT

// ====================================================
//  OTA TASK – tương thích espota.py (chuẩn handshake)
// ====================================================
static void ota_task(void *pvParameter)
{
    ESP_LOGI(OTA_TAG, "Starting OTA service on port %d", OTA_PORT);

    int serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (serverSock < 0) {
        ESP_LOGE(OTA_TAG, "Failed to create socket");
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(OTA_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        ESP_LOGE(OTA_TAG, "Bind failed");
        close(serverSock);
        vTaskDelete(NULL);
        return;
    }

    if (listen(serverSock, 1) < 0) {
        ESP_LOGE(OTA_TAG, "Listen failed");
        close(serverSock);
        vTaskDelete(NULL);
        return;
    }

    while (true) {
        ESP_LOGI(OTA_TAG, "Waiting for OTA connection...");
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSock < 0) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        ESP_LOGI(OTA_TAG, "Client connected: starting OTA update");

        // --- Handshake ---
        char header[64];
        int len = recv(clientSock, header, sizeof(header), 0);
        if (len <= 0) {
            ESP_LOGE(OTA_TAG, "Failed to receive header");
            close(clientSock);
            continue;
        }

        ESP_LOGI(OTA_TAG, "Received OTA header (%d bytes), sending OK...", len);
        send(clientSock, "OK\r\n", 4, 0);  // ✅ đúng chuẩn espota.py

        // --- Bắt đầu OTA ---
        const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
        esp_ota_handle_t ota_handle;
        esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
        if (err != ESP_OK) {
            ESP_LOGE(OTA_TAG, "esp_ota_begin failed: %s", esp_err_to_name(err));
            close(clientSock);
            continue;
        }

        uint8_t buffer[1024];
        ssize_t bytes_read;
        size_t total = 0;

        while ((bytes_read = recv(clientSock, buffer, sizeof(buffer), 0)) > 0) {
            if (esp_ota_write(ota_handle, buffer, bytes_read) != ESP_OK) {
                ESP_LOGE(OTA_TAG, "OTA write failed");
                break;
            }
            total += bytes_read;
        }

        if (esp_ota_end(ota_handle) == ESP_OK) {
            if (esp_ota_set_boot_partition(update_partition) == ESP_OK) {
                ESP_LOGI(OTA_TAG, "OTA successful, %d bytes written", (int)total);
                send(clientSock, "OK\r\n", 4, 0);
                close(clientSock);
                ESP_LOGI(OTA_TAG, "Rebooting...");
                vTaskDelay(pdMS_TO_TICKS(1000));
                esp_restart();
            } else {
                ESP_LOGE(OTA_TAG, "Failed to set boot partition");
            }
        } else {
            ESP_LOGE(OTA_TAG, "OTA end failed");
        }

        close(clientSock);
    }
}

// ====================================================
//  START OTA TASK (chạy sau khi WiFi sẵn sàng)
// ====================================================
static void start_ota_server_delayed()
{
    xTaskCreate([](void *) {
        vTaskDelay(pdMS_TO_TICKS(CONFIG_WIFI_OTA_DELAY_START));  // delay cấu hình
        xTaskCreate(ota_task, "ota_task", 8192, NULL, 5, NULL);
        vTaskDelete(NULL);
    }, "delayed_ota_start", 4096, NULL, 4, NULL);
}

// ====================================================
//  MAIN APP ENTRY
// ====================================================
extern "C" void app_main(void)
{
    // Init hệ thống cơ bản
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "Network stack initialized");

#if CONFIG_ENABLE_WIFI_OTA
    ESP_LOGI(TAG, "WiFi OTA enabled on port %d", CONFIG_WIFI_OTA_PORT);
    start_ota_server_delayed();
#else
    ESP_LOGW(TAG, "WiFi OTA disabled (set in menuconfig)");
#endif

    // Chạy ứng dụng chính
    auto &app = Application::GetInstance();
    app.Start();
}
