.. _nrf-zephyr-introduction:

nrf-Zephyr-Introduction
######

Overview
********
 This application is created for learning purpose of 
 Zephyr RTOS on nrf52. nRF52 DK board is used for this
 project but most of the nRF boards should work seamlessly
 with this project.

 Flow of Application:
 
 1. Switch 0 is used to turn on 3 onboard LEDs sequentially.
 2. Switch 1 is used to turn off 3 onboard LEDs sequentially.
 3. Push button works with interrupt and fill up Queue.
 4. Queue is continuosly read by a thread in led file, and it 
    turns on/off leds accordingly.
 5. Semaphore is used to synchronize both push buttons ISRs and thread.
 6. Mutex is used for common variables between multiple thread.

Requirements
************

nrf52 dk nrf52832 is used to run this application. You can use 
any similar board from nRF with no issues.


