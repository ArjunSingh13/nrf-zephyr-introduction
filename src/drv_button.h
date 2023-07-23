
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>


typedef void (*button_pressed_fp)(const struct device*, struct gpio_callback*, uint32_t );

void button_pressed_0(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
void button_pressed_1(const struct device *dev, struct gpio_callback *cb, uint32_t pins);

int drv_button_exti_init(const struct gpio_dt_spec* button, 
					struct gpio_callback* button_cb_data, 
					button_pressed_fp button_pressed_cb_fp);