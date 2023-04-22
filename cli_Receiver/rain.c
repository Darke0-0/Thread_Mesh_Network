#include "nrf_delay.h"
#include "dht11.h"
#include "nrf_gpio.h"

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
static void Data_SetInput()
{
        nrf_gpio_cfg_input(DHT11_PIN, NRF_GPIO_PIN_PULLUP);
}

static uint32_t Data_GetVal()
{
	return nrf_gpio_pin_read(DHT11_PIN);
}

static void Data_SetOutput()
{
        nrf_gpio_cfg_output(DHT11_PIN);
        nrf_gpio_pin_set(DHT11_PIN);
}

static void Data_ClrVal()
{
        nrf_gpio_pin_clear(DHT11_PIN);
}

void DelayUSecr(int usec)
{
       nrf_delay_us(usec);
}

void DelayMSecr(int msec)
{
      nrf_delay_ms(msec);
}

DHTxx_ErrorCode DHT_Read (uint16_t *b1 ,uint16_t *b2 ,uint16_t *b3 ,uint16_t *b4 ,uint16_t *b5)
{
  int cntr;
  int loopBits;
  uint8_t buffer[5];
  int i;
  int data;

  /* init buffer */
  for(i=0;i<sizeof(buffer); i++) {
    buffer[i] = 0;
  }

  /* set to input and check if the signal gets pulled up */
  Data_SetInput();
  DelayUSecr(50);
  if(Data_GetVal()==0) {
    return DHT11_NO_PULLUP;
  }

  /* send start signal */
  Data_SetOutput();
  Data_ClrVal();
  DelayMSecr(20); /* keep signal low for at least 18 ms */
  Data_SetInput();
  DelayUSecr(50);

  /* check for acknowledge signal */
  if (Data_GetVal()!=0) { /* signal must be pulled low by the sensor */
    return DHT11_NO_ACK_0;
  }
  /* wait max 100 us for the ack signal from the sensor */
  cntr = 18;
  while(Data_GetVal()==0) { /* wait until signal goes up */
	DelayUSecr(5);
    if (--cntr==0) {
      return DHT11_NO_ACK_1; /* signal should be up for the ACK here */
    }
  }
  /* wait until it goes down again, end of ack sequence */
  cntr = 18;
  while(Data_GetVal()!=0) { /* wait until signal goes down */
	DelayUSecr(5);
    if (--cntr==0) {
      return DHT11_NO_ACK_0; /* signal should be down to zero again here */
    }
  }
  /* now read the 40 bit data */
  i = 0;
  data = 0;
  loopBits = 40;
  do {
    cntr = 11; /* wait max 55 us */
    while(Data_GetVal()==0) {
      DelayUSecr(5);
      if (--cntr==0) {
        return DHT11_NO_DATA_0;
      }
    }
    cntr = 15; /* wait max 75 us */
    while(Data_GetVal()!=0) {
      DelayUSecr(5);
      if (--cntr==0) {
        return DHT11_NO_DATA_1;
      }
    }
    data <<= 1; /* next data bit */
    if (cntr<10) { /* data signal high > 30 us ==> data bit 1 */
      data |= 1;
    }
    if ((loopBits&0x7)==1) { /* next byte */
      buffer[i] = data;
      i++;
      data = 0;
    }
  } while(--loopBits!=0);

  /* now we have the 40 bit (5 bytes) data:
   * byte 1: humidity integer data
   * byte 2: humidity decimal data (not used for DTH11, always zero)
   * byte 3: temperature integer data
   * byte 4: temperature fractional data (not used for DTH11, always zero)
   * byte 5: checksum, the sum of byte 1 + 2 + 3 + 4
   */
  /* test CRC */

  /* store data values for caller */
  *b1 = (buffer[0]);
  *b2 = (buffer[1]);
  *b3 = (buffer[2]);
  *b4 = (buffer[3]);
  *b5 = (buffer[4]);

  return DHT11_OK;
}
