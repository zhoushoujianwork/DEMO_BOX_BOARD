#include "em_task.h"
#include "hal/led.h"
#include "hal/btn.h"
#include "hal/ble.h"
#include "hal/bat.h"
#include "hal/lvgl.h"

void loop_btn(void *pvParameters)
{
    for (;;) // A Task shall never return or exit.
    {
        vTaskDelay(10);
    }
}

void task_report(void *pvParameters)
{
    for (;;) // A Task shall never return or exit.
    {
        ble_read_from_server();
        vTaskDelay(5);
    }
}

void init_task()
{
    Serial.begin(115200);

    setup_led();
    setup_btn();
    setup_lvgl();
    // setup_bat();
    return;

    setup_ble();
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
    // long last_time = millis();
    lv_timer_handler(); // Handle LVGL tasks
    loop_btn();
    delay(5); // Wait for 5 milliseconds before the next iteration
    // lv_tick_inc(int(millis() - last_time));
}