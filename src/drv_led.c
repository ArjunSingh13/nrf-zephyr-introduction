#include <string.h>
#include "thread_common.h"
#include "drv_led.h"

typedef enum {
	maxLedOn0 = 0,
	maxLedOn1,
	maxLedOn2,
	maxLedOn3,
	ledStateMax
}eLedState;

struct gpio_dt_spec gdt_led0 = {0};
struct gpio_dt_spec gdt_led1 = {0};
struct gpio_dt_spec gdt_led2 = {0};

struct k_msgq led_MsgQ;
K_MSGQ_DEFINE(led_MsgQ, sizeof(st_queue_led_data), 10, 4);


/**
 * Pass gpio_dt_spec structure to initialize
*/
int drv_led_init(struct gpio_dt_spec* led)
{
    int ret = 0;

    if (led->port && !device_is_ready(led->port)) {
        printk("Error %d: LED device %s is not ready; ignoring it\n",
                ret, led->port->name);
        led->port = NULL;
    }

    if (led->port) {
        ret = gpio_pin_configure_dt(led, GPIO_OUTPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure LED device %s pin %d\n",
                    ret, led->port->name, led->pin);
            led->port = NULL;
        } else {
            printk("Set up LED at %s pin %d\n", led->port->name, led->pin);
        }
    }

    // Initialize local structures
    if(led->pin == LED1_PIN)
    {
        memcpy(&gdt_led0, led, sizeof(struct gpio_dt_spec));
        drv_led_reset(LED1_PIN);
    }
    else if(led->pin == LED2_PIN)
    {
        memcpy(&gdt_led1, led, sizeof(struct gpio_dt_spec));
        drv_led_reset(LED2_PIN);
    }
    else if(led->pin == LED3_PIN)
    {
        memcpy(&gdt_led2, led, sizeof(struct gpio_dt_spec));
        drv_led_reset(LED3_PIN);
    }
    
    return 1;
}

int drv_led_set(int pinNum)
{
    if(pinNum == LED1_PIN)
    {
        gpio_pin_set_dt(&gdt_led0, 1);
    }
    else if(pinNum == LED2_PIN)
    {
        gpio_pin_set_dt(&gdt_led1, 1);
    }
    else if(pinNum == LED3_PIN)
    {
        gpio_pin_set_dt(&gdt_led2, 1);
    }

    return 1;

}
int drv_led_reset(int pinNum)
{
    if(pinNum == LED1_PIN)
    {
        gpio_pin_set_dt(&gdt_led0, 0);
    }
    else if(pinNum == LED2_PIN)
    {
        gpio_pin_set_dt(&gdt_led1, 0);
    }
    else if(pinNum == LED3_PIN)
    {
        gpio_pin_set_dt(&gdt_led2, 0);
    }
    return 1;
}

void drv_led_q_send_message(st_queue_led_data* pstQLedData)
{
    k_msgq_put(&led_MsgQ, pstQLedData, K_FOREVER);
}

void thread_led(void)
{
    st_queue_led_data ledDataQueue = {0};
    
    ledDataQueue.led_state = maxLedOn0; // No led on by default
    while (1) {
        
        /* get a data item */
        k_msgq_get(&led_MsgQ, &ledDataQueue, K_FOREVER);

        switch(ledDataQueue.led_state)
        {
            case maxLedOn0:
                gpio_pin_set_dt(&gdt_led0, 0);
                gpio_pin_set_dt(&gdt_led1, 0);
                gpio_pin_set_dt(&gdt_led2, 0);   
            break;

            case maxLedOn1:
                gpio_pin_set_dt(&gdt_led0, 1);
                gpio_pin_set_dt(&gdt_led1, 0);
                gpio_pin_set_dt(&gdt_led2, 0);
            break;

            case maxLedOn2:
                gpio_pin_set_dt(&gdt_led0, 1);
                gpio_pin_set_dt(&gdt_led1, 1);
                gpio_pin_set_dt(&gdt_led2, 0);
            break;

            case maxLedOn3:
                gpio_pin_set_dt(&gdt_led0, 1);
                gpio_pin_set_dt(&gdt_led1, 1);
                gpio_pin_set_dt(&gdt_led2, 1);
            break;
        }

        k_msleep(10);
    }

}

// has to be after threads defined
K_THREAD_DEFINE(ledThread_id, LED_STACKSIZE, thread_led, NULL, NULL, NULL, LED_PRIORITY, 0, 0);