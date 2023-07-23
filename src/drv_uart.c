#include <zephyr/logging/log.h>
#include "drv_uart.h"

LOG_MODULE_REGISTER(DRV_UART, LOG_LEVEL_DBG);
static uint8_t rx_buf[10] = {0}; //A buffer to store incoming UART data
struct device *uart  = NULL;

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch (evt->type) {
	case UART_TX_DONE:
        LOG_INF("UART Data transmitted succesfully!");
		break;

	case UART_TX_ABORTED:
		// do something
		break;
	case UART_RX_RDY:
        if((evt->data.rx.len) == 1)
        {
            if(evt->data.rx.buf[evt->data.rx.offset] == '1'){
                LOG_INF("0 received over UART");
            }
            else if (evt->data.rx.buf[evt->data.rx.offset] == '2'){
                LOG_INF("1 received over UART");
            }
            else if (evt->data.rx.buf[evt->data.rx.offset] == '3'){
                LOG_INF("2 received over UART");					
            }
        }
		break;
	case UART_RX_BUF_REQUEST:
		// do something
		break;
	case UART_RX_BUF_RELEASED:
		// do something
		break;
    
    case UART_RX_DISABLED:
	    uart_rx_enable(uart, rx_buf, sizeof(rx_buf), 100);
	    break;
	case UART_RX_STOPPED:
		// do something
		break;
	default:
		break;
	}
}

/**
 * Initialize UART in DMA mode to receive and transmit data.
*/
int drv_uart_init(struct uart_config* pst_uart_cfg)
{
    uart = DEVICE_DT_GET(DT_NODELABEL(uart0));
    if (!device_is_ready(uart)) {
        return 0;
    }
   
    int err = uart_configure(uart, pst_uart_cfg);
	if (err == -ENOSYS) {
		return -ENOSYS;
	}

    // set callback for receiving UART data over DMA
    err = uart_callback_set(uart, uart_cb, NULL);
		if (err) {
			return err;
		}

    // below call is just to initialize receiving of UART data
    // 100 micro seconds here means that event UART_RX_READY is generated 
    // after 100 micro seconds of inactivity after receving data.
    uart_rx_enable(uart ,rx_buf,sizeof(rx_buf),100);
    return 1;
}

/**
 * Transmit data over UART
*/
int drv_uart_tx(char* data, int size)
{
    int err = uart_tx(uart, data, size, SYS_FOREVER_US);
	if (err) {
		return err;
	}
    return 1;
}