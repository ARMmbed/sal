/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MBED_SERIAL_API_H
#define MBED_SERIAL_API_H

#include "device.h"
#include "buffer.h"
#include "dma_api.h"

#if DEVICE_SERIAL

#define SERIAL_EVENT_TX_SHIFT (2)
#define SERIAL_EVENT_RX_SHIFT (8)

#define SERIAL_EVENT_TX_MASK (0x00FC)
#define SERIAL_EVENT_RX_MASK (0x3F00)

#define SERIAL_EVENT_ERROR (1 << 1)

/**
 * @defgroup SerialTXEvents Serial TX Events Macros
 *
 * @{
 */
#define SERIAL_EVENT_TX_COMPLETE (1 << (SERIAL_EVENT_TX_SHIFT + 0))
#define SERIAL_EVENT_TX_ALL      (SERIAL_EVENT_TX_COMPLETE)
/**@}*/

/**
 * @defgroup SerialRXEvents Serial RX Events Macros
 *
 * @{
 */
#define SERIAL_EVENT_RX_COMPLETE        (1 << (SERIAL_EVENT_RX_SHIFT + 0))
#define SERIAL_EVENT_RX_OVERRUN_ERROR   (1 << (SERIAL_EVENT_RX_SHIFT + 1))
#define SERIAL_EVENT_RX_FRAMING_ERROR   (1 << (SERIAL_EVENT_RX_SHIFT + 2))
#define SERIAL_EVENT_RX_PARITY_ERROR    (1 << (SERIAL_EVENT_RX_SHIFT + 3))
#define SERIAL_EVENT_RX_OVERFLOW        (1 << (SERIAL_EVENT_RX_SHIFT + 4))
#define SERIAL_EVENT_RX_CHARACTER_MATCH (1 << (SERIAL_EVENT_RX_SHIFT + 5))
#define SERIAL_EVENT_RX_ALL             (SERIAL_EVENT_RX_OVERFLOW | SERIAL_EVENT_RX_PARITY_ERROR | \
                                         SERIAL_EVENT_RX_FRAMING_ERROR | SERIAL_EVENT_RX_OVERRUN_ERROR \
                                         SERIAL_EVENT_RX_COMPLETE | SERIAL_EVENT_RX_CHARACTER_MATCH)
/**@}*/

#define SERIAL_RESERVED_CHAR_MATCH (255)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ParityNone = 0,
    ParityOdd = 1,
    ParityEven = 2,
    ParityForced1 = 3,
    ParityForced0 = 4
} SerialParity;

typedef enum {
    RxIrq,
    TxIrq
} SerialIrq;

typedef enum {
    FlowControlNone,
    FlowControlRTS,
    FlowControlCTS,
    FlowControlRTSCTS
} FlowControl;

typedef void (*uart_irq_handler)(uint32_t id, SerialIrq event);

typedef struct {
    struct serial_s serial;
    struct buffer_s tx_buff;
    struct buffer_s rx_buff;
    uint8_t char_match;
    uint8_t char_found;
} serial_t;

/**
 * \defgroup GeneralSerial Serial Configuration Functions
 * @{
 */

/** Initialize the serial peripheral. It sets the default parameters for serial
 *  peripheral, and configure its specifieds pins.
 *
 * @param obj The serial object
 * @param tx  The TX pin
 * @param rx  The RX pin
 */
void serial_init(serial_t *obj, PinName tx, PinName rx);

/** Release the serial peripheral, not currently invoked. It requires further
 *  resource management.
 *
 * @param obj The serial object
 */
void serial_free(serial_t *obj);

/** Configure the baud rate
 *
 * @param obj      The serial object
 * @param baudrate The baud rate to be configured
 */
void serial_baud(serial_t *obj, int baudrate);

/** Configure the format. Set the number of bits, parity and the number of stop bits
 *
 * @param obj       The serial object
 * @param data_bits The number of data bits
 * @param parity    The parity
 * @param stop_bits The number of stop bits
 */
void serial_format(serial_t *obj, int data_bits, SerialParity parity, int stop_bits);

/** The serial interrupt handler registration.
 *
 * @param obj     The serial object
 * @param handler The interrupt handler which will be invoked when interrupt fires.
 * @param id      The SerialBase object
 */
void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id);

/** Configure serial interrupt. This function is used for word-approach
 *
 * @param obj    The serial object
 * @param irq    The serial IRQ type (RX or TX)
 * @param enable Set to non-zero to enable events, or zero to disable them
 */
void serial_irq_set(serial_t *obj, SerialIrq irq, uint32_t enable);

/** Get character. This is a blocking call, waiting for a character
 *
 * @param obj The serial object
 */
int  serial_getc(serial_t *obj);

/** Put a character. This is a blocking call, waiting for a peripheral to be available
 *  for writing
 *
 * @param obj The serial object
 * @param c   The character to be sent
 */
void serial_putc(serial_t *obj, int c);

/** Check if the serial peripheral is readable
 *
 * @param obj The serial object
 * @return Non-zero value if a character can be read, 0 if nothing to read.
 */
int  serial_readable(serial_t *obj);

/** Check if the serial peripheral is writable
 *
 * @param obj The serial object
 * @return Non-zero value if a character can be written, 0 otherwise.
 */
int  serial_writable(serial_t *obj);

/** Clear the serial peripheral
 *
 * @param obj The serial object
 */
void serial_clear(serial_t *obj);

/** Set the break
 *
 * @param obj The serial object
 */
void serial_break_set(serial_t *obj);

/** Clear the break
 *
 * @param obj The serial object
 */
void serial_break_clear(serial_t *obj);

/** Configure the TX pin for UART function.
 *
 * @param tx The pin used for TX
 */
void serial_pinout_tx(PinName tx);

/** Configure the serial for the flow control. It sets flow control in the hardware
 *  if a serial peripheral supports it, otherwise software emulation is used.
 *
 * @param obj    The serial object
 * @param type   The type of the flow control. Look at the available FlowControl types.
 * @param rxflow The tx pin
 * @param txflow The rx pin
 */
void serial_set_flow_control(serial_t *obj, FlowControl type, PinName rxflow, PinName txflow);

/**@}*/

/**
 * \defgroup AsynchSerial Asynchronous serial Hardware Abstraction Layer
 * @{
 */

/** Begin asynchronous TX transfer. The used buffer is specified in the serial object,
 *  tx_buff
 *
 * @param obj  The serial object
 * @param cb   The function to call when an event occurs
 * @param hint A suggestion for how to use DMA with this transfer
 * @return Returns number of data transfered, or 0 otherwise
 */
int serial_start_write_asynch(serial_t *obj, void* cb, DMA_USAGE_Enum hint);

/** Begin asynchronous RX transfer (enable interrupt for data collecting)
 *  The used buffer is specified in the serial object - rx_buff
 *
 * @param obj  The serial object
 * @param cb   The function to call when an event occurs
 * @param hint A suggestion for how to use DMA with this transfer
 */
void serial_start_read_asynch(serial_t *obj, void* cb, DMA_USAGE_Enum hint);

/** Configure TX events
 *
 * @param obj    The serial object
 * @param event  The logical OR of the TX events to configure
 * @param enable Set to non-zero to enable events, or zero to disable them
 */
void serial_tx_enable_event(serial_t *obj, uint32_t event, uint8_t enable);

/**
 * @param obj    The serial object.
 * @param event  The logical OR of the RX events to configure
 * @param enable Set to non-zero to enable events, or zero to disable them
 */
void serial_rx_enable_event(serial_t *obj, uint32_t event, uint8_t enable);

/** Configure the TX buffer for an asynchronous write serial transaction
 *
 * @param obj       The serial object.
 * @param tx        The buffer for sending.
 * @param tx_length The number of words to transmit.
 */
void serial_tx_buffer_set(serial_t *obj, void *tx, uint32_t tx_length);

/** Configure the TX buffer for an asynchronous read serial transaction
 *
 * @param obj       The serial object.
 * @param rx        The buffer for receiving.
 * @param rx_length The number of words to read.
 */
void serial_rx_buffer_set(serial_t *obj, void *rx, uint32_t tx_length);

/** Attempts to determine if the serial peripheral is already in use for TX
 *
 * @param obj The serial object
 * @return Non-zero if the RX transaction is ongoing, 0 otherwise
 */
uint8_t serial_tx_active(serial_t *obj);

/** Attempts to determine if the serial peripheral is already in use for RX
 *
 * @param obj The serial object
 * @return Non-zero if the RX transaction is ongoing, 0 otherwise
 */
uint8_t serial_rx_active(serial_t *obj);

/** The asynchronous TX handler. Writes to the TX FIFO and checks for events.
 *  If any TX event has occured, the TX abort function is called.
 *
 * @param obj The serial object
 * @return Returns event flags if a TX transfer termination condition was met or 0 otherwise
 */
uint32_t serial_tx_irq_handler_asynch(serial_t *obj);

/** The asynchronous RX handler. Reads from the RX FIFOF and checks for events.
 *  If any RX event has occured, the RX abort function is called.
 *
 * @param obj The serial object
 * @return Returns event flags if a RX transfer termination condition was met or 0 otherwise
 */
uint32_t serial_rx_irq_handler_asynch(serial_t *obj);

/** Abort the ongoing TX transaction. It disables the enabled interupt for TX and
 *  flush TX hardware buffer if TX FIFO is used
 *
 * @param obj The serial object
 */
void serial_tx_abort_asynch(serial_t *obj);

/** Abort the ongoing RX transaction It disables the enabled interrupt for RX and
 *  flush RX hardware buffer if RX FIFO is used
 *
 * @param obj The serial object
 */
void serial_rx_abort_asynch(serial_t *obj);

/** Enable and set the interrupt handler for write (TX)
 *
 * @param obj     The serial object
 * @param address The address of TX handler
 * @param enable  Set to non-zero to enable or zero to disable
 */
void serial_write_enable_interrupt(serial_t *obj, uint32_t address, uint8_t enable);

/** Enable and set the interrupt handler for read (RX)
 *
 * @param obj     The serial object
 * @param address The address of RX handler
 * @param enable  Set to non-zero to enable or zero to disable
 */
void serial_read_enable_interrupt(serial_t *obj, uint32_t address, uint8_t enable);

/** Set character to be matched. If an event is enabled, and received character
 *  matches the defined char_match, the receiving process is stopped and MATCH event
 *  is invoked
 *
 * @param obj        The serial object
 * @param char_match A character in range 0-254
 */
void serial_set_char_match(serial_t *obj, uint8_t char_match);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif

#endif
