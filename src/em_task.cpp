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
        loop_btn();
    }
}

void task_report(void *pvParameters)
{
    for (;;) // A Task shall never return or exit.
    {
        ble_read_from_server(); // 会阻塞
        vTaskDelay(5);
    }
}

void init_task()
{
    Serial.begin(115200);

    setup_ble();
    setup_led();
    setup_btn();
    setup_lvgl();
    // return;
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
    if (0)
    {
        int32_t speed = random(0, 199);
        // Serial.printf("_set_arc_value %d", speed);
        _set_arc_value(speed);
    }
    else
    {
        ui_flash();
    }

    lv_timer_handler(); // Handle LVGL tasks
    delay(500);         // Wait for 5 milliseconds before the next iteration
}