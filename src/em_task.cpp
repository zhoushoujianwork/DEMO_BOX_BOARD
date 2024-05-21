#include "em_task.h"
#include "hal/led.h"
#include "hal/btn.h"
#include "hal/ble.h"
#include "hal/bat.h"
#include "hal/lvgl.h"

void loop_led(void *pvParameters)
{
    for (;;) // A Task shall never return or exit.
    {
        loop_led();
        vTaskDelay(100);
    }
}

void loop_btn(void *pvParameters)
{
    for (;;) // A Task shall never return or exit.
    {
        loop_btn();
        vTaskDelay(10);
    }
}

void task_report(void *pvParameters)
{
    for (;;) // A Task shall never return or exit.
    {
        loop_ble();
        vTaskDelay(1000);
    }
}

void init_task()
{
    Serial.begin(115200);

    setup_led();
    setup_bat();
    setup_btn();
    setup_lvgl();
    setup_ble();

    xTaskCreate(
        loop_led,   // 任务函数
        "LED_TASK", // 任务名
        1024 * 10,  // 任务栈
        NULL,       // 任务参数
        1,          // 任务优先级, with 3 (configMAX_PRIORITIES - 1) 是最高的，0是最低的.
        NULL        // 任务句柄
    );
    xTaskCreate(
        loop_btn,   // 任务函数
        "BTN_TASK", // 任务名
        1024 * 10,  // 任务栈
        NULL,       // 任务参数
        1,          // 任务优先级, with 3 (configMAX_PRIORITIES - 1) 是最高的，0是最低的.
        NULL        // 任务句柄
    );
    xTaskCreate(
        task_report,  // 任务函数
        "TaskReport", // 任务名
        1024 * 10,    // 任务栈
        NULL,         // 任务参数
        1,            // 任务优先级, with 3 (configMAX_PRIORITIES - 1) 是最高的，0是最低的.
        NULL          // 任务句柄
    );

    Serial.println("init_task ok");
}

void loop_task()
{
}