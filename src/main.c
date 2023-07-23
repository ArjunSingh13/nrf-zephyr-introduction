/**
 * This application is created for learning purpose of 
 * Zephyr RTOS on nrf52. nRF52 DK board is used for this
 * project but most of the nRF boards should work seamlessly
 * with this project.
 * 
 * Flow of Application:
 * 
 * 1. Switch 0 is used to turn on 3 onboard LEDs sequentially.
 * 2. Switch 1 is used to turn off 3 onboard LEDs sequentially.
 * 3. Push button works with interrupt and fill up Queue.
 * 4. Queue is continuosly read by a thread in led file, and it 
 *    turns on/off leds accordingly.
 * 5. Semaphore is used to synchronize both push buttons ISRs and thread.
 * 6. Mutex is used for common variables between multiple thread.
 * 
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

/** Included from SDK */
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>

/** Custom libraries */
#include "thread_common.h"
#include "drv_button.h"
#include "drv_led.h"
#include "drv_uart.h"

#define SLEEP_TIME_MS	100000
LOG_MODULE_REGISTER(button_tutorial, LOG_LEVEL_DBG);

/*********************** Button Initialization *********************/
/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(SW1_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

const struct gpio_dt_spec button0 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios, {0});

struct gpio_callback button_cb_data_0;
struct gpio_callback button_cb_data_1;

/*********************** LED Initialization ***********************/
/**
 * The led0 devicetree alias is converted to node ID and converted
 * to Device pointer.
 */
struct gpio_dt_spec dt_led0 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});
struct gpio_dt_spec dt_led1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led1), gpios, {0});
struct gpio_dt_spec dt_led2 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led2), gpios, {0});

/************************** UART **************************/
static uint8_t tx_buf[] =  {"\n\r-------->Sup from nRF!<--------- \n\r"
                            "Press Switch 0 to turn on LED1-LED3  \n\r"
							"Press Switch 1 to turn off LED1-LED3 \n\r"};

int main(void)
{
	int ret;

	LOG_INF("Going to initialize push button 0");
 	ret = drv_button_exti_init(&button0, &button_cb_data_0, button_pressed_0);
	if(!ret)
	{
		LOG_ERR("Push button 0 initialization failed");
	}

	LOG_INF("Going to initialize push button 1");
 	ret = drv_button_exti_init(&button1, &button_cb_data_1, button_pressed_1);
	if(!ret)
	{
		LOG_ERR("Push button 1 initialization failed");
	}

	LOG_INF("Going to initialize LED");
	ret = drv_led_init(&dt_led0);
	if(!ret)
	{
		LOG_ERR("led0 initialization failed");
	}
	
	ret = drv_led_init(&dt_led1);
	if(!ret)
	{
		LOG_ERR("led1 initialization failed");
	}
	
	ret = drv_led_init(&dt_led2);
	if(!ret)
	{
		LOG_ERR("led2 initialization failed");
	}

    // Default settings
    struct uart_config uart_cfg = {
		.baudrate = 115200,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE
	};

	ret = drv_uart_init(&uart_cfg);
	if(!ret)
	{
		LOG_ERR("UART initialization failed");
	}
	
	// Not required, just some delay before printing on console
	k_msleep(500);
	
#if 1
	drv_uart_tx(tx_buf, sizeof(tx_buf));
#endif

	while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}