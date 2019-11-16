/*
 * mbed SDK
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Automatically generated configuration file.
// DO NOT EDIT, content will be overwritten.

#ifndef __MBED_CONFIG_DATA__
#define __MBED_CONFIG_DATA__

// Configuration parameters
#define CLOCK_SOURCE                                                      USE_PLL_HSE_XTAL|USE_PLL_HSI            // set by target:STM32F407VE_BLACK
#define LPTICKER_DELAY_TICKS                                              1                                       // set by target:FAMILY_STM32
#define MBED_CONF_ATMEL_RF_ASSUME_SPACED_SPI                              1                                       // set by library:atmel-rf[STM]
#define MBED_CONF_ATMEL_RF_FULL_SPI_SPEED                                 7500000                                 // set by library:atmel-rf
#define MBED_CONF_ATMEL_RF_FULL_SPI_SPEED_BYTE_SPACING                    250                                     // set by library:atmel-rf
#define MBED_CONF_ATMEL_RF_IRQ_THREAD_STACK_SIZE                          1024                                    // set by library:atmel-rf
#define MBED_CONF_ATMEL_RF_LOW_SPI_SPEED                                  3750000                                 // set by library:atmel-rf
#define MBED_CONF_ATMEL_RF_PROVIDE_DEFAULT                                0                                       // set by library:atmel-rf
#define MBED_CONF_ATMEL_RF_USE_SPI_SPACING_API                            0                                       // set by library:atmel-rf
#define MBED_CONF_DRIVERS_UART_SERIAL_RXBUF_SIZE                          256                                     // set by library:drivers
#define MBED_CONF_DRIVERS_UART_SERIAL_TXBUF_SIZE                          256                                     // set by library:drivers
#define MBED_CONF_ESP8266_DEBUG                                           0                                       // set by library:esp8266
#define MBED_CONF_ESP8266_POWER_OFF_TIME_MS                               3                                       // set by library:esp8266
#define MBED_CONF_ESP8266_POWER_ON_POLARITY                               0                                       // set by library:esp8266
#define MBED_CONF_ESP8266_POWER_ON_TIME_MS                                3                                       // set by library:esp8266
#define MBED_CONF_ESP8266_PROVIDE_DEFAULT                                 0                                       // set by library:esp8266
#define MBED_CONF_ESP8266_SERIAL_BAUDRATE                                 115200                                  // set by library:esp8266
#define MBED_CONF_ESP8266_SOCKET_BUFSIZE                                  8192                                    // set by library:esp8266
#define MBED_CONF_EVENTS_PRESENT                                          1                                       // set by library:events
#define MBED_CONF_EVENTS_SHARED_DISPATCH_FROM_APPLICATION                 0                                       // set by library:events
#define MBED_CONF_EVENTS_SHARED_EVENTSIZE                                 768                                     // set by library:events
#define MBED_CONF_EVENTS_SHARED_HIGHPRIO_EVENTSIZE                        256                                     // set by library:events
#define MBED_CONF_EVENTS_SHARED_HIGHPRIO_STACKSIZE                        1024                                    // set by library:events
#define MBED_CONF_EVENTS_SHARED_STACKSIZE                                 2048                                    // set by library:events
#define MBED_CONF_EVENTS_USE_LOWPOWER_TIMER_TICKER                        0                                       // set by library:events
#define MBED_CONF_FILESYSTEM_PRESENT                                      1                                       // set by library:filesystem
#define MBED_CONF_MCR20A_PROVIDE_DEFAULT                                  0                                       // set by library:mcr20a
#define MBED_CONF_PLATFORM_CRASH_CAPTURE_ENABLED                          0                                       // set by library:platform
#define MBED_CONF_PLATFORM_CTHUNK_COUNT_MAX                               8                                       // set by library:platform
#define MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE                       9600                                    // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_ALL_THREADS_INFO                         0                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_FILENAME_CAPTURE_ENABLED                 0                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_HIST_ENABLED                             0                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_HIST_SIZE                                4                                       // set by library:platform
#define MBED_CONF_PLATFORM_ERROR_REBOOT_MAX                               1                                       // set by library:platform
#define MBED_CONF_PLATFORM_FATAL_ERROR_AUTO_REBOOT_ENABLED                0                                       // set by library:platform
#define MBED_CONF_PLATFORM_FORCE_NON_COPYABLE_ERROR                       0                                       // set by library:platform
#define MBED_CONF_PLATFORM_MAX_ERROR_FILENAME_LEN                         16                                      // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_64_BIT                   1                                       // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_ENABLE_FLOATING_POINT           0                                       // set by library:platform
#define MBED_CONF_PLATFORM_MINIMAL_PRINTF_SET_FLOATING_POINT_MAX_DECIMALS 6                                       // set by library:platform
#define MBED_CONF_PLATFORM_POLL_USE_LOWPOWER_TIMER                        0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_BAUD_RATE                                115200                                  // set by application[*]
#define MBED_CONF_PLATFORM_STDIO_BUFFERED_SERIAL                          0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES                         0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_TTY_NEWLINES                     0                                       // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT                            1                                       // set by library:platform
#define MBED_CONF_PLATFORM_USE_MPU                                        1                                       // set by library:platform
#define MBED_CONF_RTOS_API_PRESENT                                        1                                       // set by library:rtos-api
#define MBED_CONF_RTOS_IDLE_THREAD_STACK_SIZE                             512                                     // set by library:rtos
#define MBED_CONF_RTOS_IDLE_THREAD_STACK_SIZE_DEBUG_EXTRA                 128                                     // set by library:rtos[STM]
#define MBED_CONF_RTOS_IDLE_THREAD_STACK_SIZE_TICKLESS_EXTRA              256                                     // set by library:rtos
#define MBED_CONF_RTOS_MAIN_THREAD_STACK_SIZE                             4096                                    // set by library:rtos
#define MBED_CONF_RTOS_PRESENT                                            1                                       // set by library:rtos
#define MBED_CONF_RTOS_THREAD_STACK_SIZE                                  4096                                    // set by library:rtos
#define MBED_CONF_RTOS_TIMER_THREAD_STACK_SIZE                            768                                     // set by library:rtos
#define MBED_CONF_S2LP_PROVIDE_DEFAULT                                    0                                       // set by library:s2lp
#define MBED_CONF_SDIO_CMD_TIMEOUT                                        30000                                   // set by library:sdio
#define MBED_CONF_SDIO_FSFAT_SDCARD_INSTALLED                             1                                       // set by library:sdio
#define MBED_CONF_TARGET_BOOT_STACK_SIZE                                  0x400                                   // set by library:rtos[*]
#define MBED_CONF_TARGET_CONSOLE_UART                                     1                                       // set by target:Target
#define MBED_CONF_TARGET_DEEP_SLEEP_LATENCY                               3                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_INIT_US_TICKER_AT_BOOT                           1                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_LPTICKER_LPTIM_CLOCK                             1                                       // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_LPUART_CLOCK_SOURCE                              USE_LPUART_CLK_LSE|USE_LPUART_CLK_PCLK1 // set by target:FAMILY_STM32
#define MBED_CONF_TARGET_LSE_AVAILABLE                                    0                                       // set by target:STM32F407VE_BLACK
#define MBED_CONF_TARGET_MPU_ROM_END                                      0x0fffffff                              // set by target:Target
#define MBED_CONF_TARGET_TICKLESS_FROM_US_TICKER                          0                                       // set by target:Target
#define MBED_CPU_STATS_ENABLED                                            1                                       // set by application[*]
#define MBED_LFS_BLOCK_SIZE                                               512                                     // set by library:littlefs
#define MBED_LFS_ENABLE_INFO                                              0                                       // set by library:littlefs
#define MBED_LFS_INTRINSICS                                               1                                       // set by library:littlefs
#define MBED_LFS_LOOKAHEAD                                                512                                     // set by library:littlefs
#define MBED_LFS_PROG_SIZE                                                64                                      // set by library:littlefs
#define MBED_LFS_READ_SIZE                                                64                                      // set by library:littlefs
#define NVSTORE_ENABLED                                                   1                                       // set by library:nvstore
#define NVSTORE_MAX_KEYS                                                  16                                      // set by library:nvstore
// Macros
#define _RTE_                                                                                                     // defined by library:rtos

#endif
