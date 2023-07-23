#if 0
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>
#endif

#include <zephyr/drivers/uart.h>

int drv_uart_init(struct uart_config* pst_uart_cfg);
int drv_uart_tx(char* data, int size);