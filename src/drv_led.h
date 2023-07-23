#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


#define LED1_PIN    17
#define LED2_PIN    18
#define LED3_PIN    19

typedef struct
{
	uint16_t led_state;
} __attribute__((aligned(4))) st_queue_led_data;

int drv_led_init(struct gpio_dt_spec* led);
int drv_led_set(int pinNum);
int drv_led_reset(int pinNum);
void drv_led_q_send_message(st_queue_led_data* pstQLedData);