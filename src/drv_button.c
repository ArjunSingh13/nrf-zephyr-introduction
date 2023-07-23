#include <zephyr/logging/log.h>

#include "thread_common.h"
#include "drv_button.h"
#include "drv_led.h"

LOG_MODULE_REGISTER(drv_button, LOG_LEVEL_DBG);
typedef enum {
	maxLedOn0 = 0,
	maxLedOn1,
	maxLedOn2,
	maxLedOn3,
	maxLedAvailable
}eLedState;

K_SEM_DEFINE(sem_button_0, 0, 1);
K_SEM_DEFINE(sem_button_1, 0, 1);
K_MUTEX_DEFINE(button_mutex);

st_queue_led_data gst_queue_data = {0};
static int8_t gu8_led_num = 0;

void button_pressed_0(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_sem_give(&sem_button_0);
}

void button_pressed_1(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	k_sem_give(&sem_button_1);
}

/**
 * Initialize the button number passed into argument
*/
int drv_button_exti_init(const struct gpio_dt_spec* button, 
					struct gpio_callback* button_cb_data, 
					button_pressed_fp button_pressed_cb_fp)
{
    int ret = 1;

    /** Check if button pin is supported in device tree*/
	if (!gpio_is_ready_dt(button)) {
		printk("Error: button device %s is not ready\n",
		       button->port->name);
		return 0;
	}

    /** Configure the button for input */
	ret = gpio_pin_configure_dt(button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button->port->name, button->pin);
		return 0;
	}

    /** Configure the button pin for interrupt with edge to high */
	ret = gpio_pin_interrupt_configure_dt(button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button->port->name, button->pin);
		return 0;
	}

    /** Initialize callback with button_pressed function passed to it */
	gpio_init_callback(button_cb_data, button_pressed_cb_fp, BIT(button->pin));

    /** Add into callback */
	gpio_add_callback(button->port, button_cb_data);
	
	// No leds on initially
	gst_queue_data.led_state = 0;

    /** Print back that button initialization is successful */
    printk("Set up button at %s pin %d\n", button->port->name, button->pin);


    return 1;
}

/**
 * To turn on on board LEDs, one at a time.
*/
void thread_button0(void)
{
	printk("Going to run thread 0");

    while (1) {
		k_sem_take(&sem_button_0, K_FOREVER);
		LOG_INF("Button 0 pressed at %" PRIu32 "\n", k_cycle_get_32());
		k_mutex_lock(&button_mutex, K_FOREVER);
		gu8_led_num++;
		if(gu8_led_num < maxLedAvailable)
		{
			switch(gu8_led_num)
			{
				case maxLedOn1:
					// Send message to queue to turn on led 0;
					gst_queue_data.led_state = maxLedOn1;
					break;

				case maxLedOn2:
					gst_queue_data.led_state = maxLedOn2;
					break;

				case maxLedOn3:
					gst_queue_data.led_state = maxLedOn3;
					break;

				default:
					// print maybe cant turn on no more leds	 
			}

			drv_led_q_send_message(&gst_queue_data);
		}
		else if (gu8_led_num > maxLedOn3)
		{
			gu8_led_num = maxLedOn3;
			//TODO: Print to workqueue thread
		}

		k_mutex_unlock(&button_mutex);
		k_msleep(20);
    }   

}

/**
 * To turn off on board LEDs, one at a time.
*/
void thread_button1(void)
{
    while (1) {
		k_sem_take(&sem_button_1, K_FOREVER);
		LOG_INF("Button 1 pressed at %" PRIu32 "\n", k_cycle_get_32());		
		k_mutex_lock(&button_mutex, K_FOREVER);
		gu8_led_num--;
		if(gu8_led_num >= 0 && gu8_led_num < maxLedAvailable)
		{
			switch(gu8_led_num)
			{
				case maxLedOn0:
					// Send message to queue to turn on led 0;
					gst_queue_data.led_state = maxLedOn0;
					break;

				case maxLedOn1:
					gst_queue_data.led_state = maxLedOn1;
					break;

				case maxLedOn2:
					gst_queue_data.led_state = maxLedOn2;
					break;

				default:
					// print maybe cant turn on no more leds	 
			}
			// send message to queue
			drv_led_q_send_message(&gst_queue_data);
		}
		else if (gu8_led_num < 0)
		{
			gu8_led_num = maxLedOn0;
			// TODO: print to workqueue thread
		}
		k_mutex_unlock(&button_mutex);
        k_msleep(20);
    } 
}

// has to be after threads defined
K_THREAD_DEFINE(thread0_id, BUTTON0_STACKSIZE, thread_button0, NULL, NULL, NULL, BUTTON0_PRIORITY, 0, 0);
K_THREAD_DEFINE(thread1_id, BUTTON1_STACKSIZE, thread_button1, NULL, NULL, NULL, BUTTON1_PRIORITY, 0, 0);