#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int total = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < total; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int received = 0;
    static int window[5] = {0};
    static int pos = 0;
    static int total_sum = 0;

    while (1) {
        if (xQueueReceive(xQueueData, &received, 100)) {
            total_sum -= window[pos];

            window[pos] = received;

            total_sum += received;

            int filtered = total_sum / 5;

            printf("%d\n", filtered);

            pos = (pos + 1) % 5;

            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data Task", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Processing Task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
