#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <driver/uart.h>
#include <esp_task_wdt.h>

#define DELAY 500
#define LED_PIN GPIO_NUM_2
#define ANALOG_CHANNEL ADC1_CHANNEL_0
#define LED_BUILTIN 2
#define UART_NUM UART_NUM_0
#define BUF_SIZE 1024

#define DELAY_TO_TICKS(DELAY) ((TickType_t)((DELAY) / portTICK_PERIOD_MS))

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t TaskBlink_Handler;
TaskHandle_t TaskSerial_Handler;

void Task1code(void *pvParameters);
void Task2code(void *pvParameters);
void taskOne(void* pvParameters);
void taskTwo(void* pvParameters);
void taskThree(void* pvParameters);
void TaskBlink(void *pvParameters);
void TaskSerial(void *pvParameters);

void taskA(void);
void taskB(void);
void taskC(void);
void taskD(void);

void app_main(void)
{
    taskD();

    while(1) {
        ;
    }
}

void taskA(void) {
        printf("app_main is running on core %d\n", xPortGetCoreID());

    while(1) {
        printf("loop() running on core %d\n", xPortGetCoreID());
        vTaskDelay(DELAY_TO_TICKS(DELAY));
    }
}

void taskB(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);
    xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 0);
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, &Task2, 1);

    while(1) {
        ;
    }
}

void taskC(void) {
    vTaskDelay(DELAY_TO_TICKS(1000));

    xTaskCreatePinnedToCore(taskOne, "TaskOne", 10000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskTwo, "TaskTwo", 10000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(taskThree, "TaskThree", 10000, NULL, 3, NULL, 0);

    while(1) {

    }
}

void taskD(void) {
    esp_task_wdt_delete(NULL);

    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, BUF_SIZE*2, 0, 0, NULL, 0);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    xTaskCreatePinnedToCore(TaskBlink, "Blink", 10000, NULL, 2, &TaskBlink_Handler, 0);
    xTaskCreatePinnedToCore(TaskSerial, "Serial", 10000, NULL, 1, &TaskSerial_Handler, 0);

    while(1) {

    }
}

void Task1code(void *pvParameters) {
    printf("Task1 is running on core %d\n", xPortGetCoreID());

    while(1) {
        gpio_set_level(LED_PIN, 1);
        printf("Hello Task1\n");
        vTaskDelay(DELAY_TO_TICKS(DELAY));
        gpio_set_level(LED_PIN, 0);
        printf("Hello Task1\n");
        vTaskDelay(DELAY_TO_TICKS(DELAY));
    }
}

void Task2code(void *pvParameters) {
    printf("Task2 is running on core %d\n", xPortGetCoreID());
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ANALOG_CHANNEL, ADC_ATTEN_DB_12);

    while(1) {
        int adc_channel = adc1_get_raw(ANALOG_CHANNEL);
        printf("ADC Value: %d\n", adc_channel);
        printf("Hello Task2\n");
        vTaskDelay(DELAY_TO_TICKS(DELAY));
    }
}

void taskOne(void* pvParameters) {
    volatile int dummy = 0;
    while(1) {
        for (int i = 0; i < 2000000; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                dummy++;
            }
            
        }

        printf("%d\n", dummy);
        printf("Running task 1\n");

        vTaskDelay(DELAY_TO_TICKS(5000));
        
    }
}

void taskTwo(void* pvParameters) {
    volatile int dummy2 = 0;
    while(1) {
        for (int i = 0; i < 2000000; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                dummy2++;
            }
            
        }

        printf("%d\n", dummy2);
        printf("Running task 2\n");

        vTaskDelay(DELAY_TO_TICKS(5000));
        
    }
}

void taskThree(void* pvParameters) {
    volatile int dummy3 = 0;
    while(1) {
        for (int i = 0; i < 2000000; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                dummy3++;
            }
            
        }

        printf("%d\n", dummy3);
        printf("Running task 3\n");

        vTaskDelay(DELAY_TO_TICKS(5000));
        
    }
}

void TaskSerial(void *pvParameters) {
    uint8_t data[BUF_SIZE];


    while (1)
    {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE-1, DELAY_TO_TICKS(10));
        if (len > 0) {
            data[len] = '\0';
            switch (data[0])
            {
            case 's':
                printf("Suspend!\n");
                vTaskSuspend(TaskBlink_Handler);
                break;
            case 'r':
                vTaskResume(TaskBlink_Handler);
                printf("Resume!\n");
                break;
            default:
                printf("Unknown Command: %c", data[0]);
                break;
            }
        }
        vTaskDelay(1);
    }
    
}

void TaskBlink(void *pvParameters) {
    (void)pvParameters;
    while(1) {
        printf("BLINKY\n");
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(DELAY_TO_TICKS(1000));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(DELAY_TO_TICKS(1000));
    }
}