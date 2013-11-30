/*
 * Synchronous DHT-11 temp & humidity sensor module reader
 *
 *
 * Copyright (c) 2012, Angus Gratton. Licensed under the Modified BSD License.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "dht11.h"
#include "ch.h"
#include "hal.h"


void init_dht11() {
 /* //INPUT(DHT_PIN);
	 palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_INPUT);
	// palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_OUTPUT_PUSHPULL);
  // use internal 20k pullup (lazy!)
 // HIGH(DHT_PIN);
	palSetPad(DHTPORT, DHT_PIN);*/
 	palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_OUTPUT_PUSHPULL); //output
	palSetPad(DHTPORT, DHT_PIN); //high
	chThdSleepMilliseconds(100);
}

union _dht_output {
  uint8_t bytes[5];
  struct {
    dht11_data inner;
    uint8_t checksum;
  } data;
};

/* Wait for a DHT signal edge, high or low. Edge must be at least min_length us and no longer than max_length us.
 *
 * Returns the edge duration, or 0 if the edge was outside of limits (too long or too short)
 */
static uint8_t listen_edge(bool high, uint8_t min_length, uint8_t max_length)
{
  uint8_t wait_time;
  for(wait_time = 0; wait_time < max_length && (palReadPad(DHTPORT, DHT_PIN) == high); wait_time++)
   chThdSleepMicroseconds(1);
  if(wait_time == max_length || wait_time < min_length)
    return 0; // bad edge
  return wait_time;
}

dht11_status read_dht11(dht11_data *data) {
  //OUTPUT(DHT_PIN);
	 palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_OUTPUT_PUSHPULL);
  //LOW(DHT_PIN); // send DHT wakeup signal
	 palClearPad(DHTPORT, DHT_PIN);
  //_delay_ms(18);
	chThdSleepMilliseconds(18);
  //HIGH(DHT_PIN);
	palSetPad(DHTPORT, DHT_PIN);
  //_delay_us(40);
	chThdSleepMicroseconds(1);
 // INPUT(DHT_PIN);
	 palSetPadMode(DHTPORT, DHT_PIN, PAL_MODE_INPUT);
	chThdSleepMicroseconds(39);
  if(!listen_edge(false, 0, 100)) // hold for DHT  ack signal, 80us
    return bad_ack;

  //_delay_us(5);
	chThdSleepMicroseconds(5);
  if(!listen_edge(true, 0, 100)) // pre-transmission pause from DHT
    return bad_post_ack;

  union _dht_output raw = { .bytes={0} };
  for(uint8_t byte = 0; byte < sizeof(raw); byte++) {
    for(int8_t bit = 7; bit >= 0; bit--) {
      if(!listen_edge(false, 20, 80)) // 50us bit prelude
        return bad_bit_prelude;
      uint8_t bit_length = listen_edge(true, 10, 90); // bit signal - 26-28us for 0 70us for 1
      if(!bit_length)
        return bad_bit_data_pause; // invalid data pulse
      if(bit_length > 30)
        raw.bytes[byte] |= 1<<bit;
    }
  }

  if(raw.data.checksum != (uint8_t)(raw.bytes[0]+raw.bytes[1]+raw.bytes[2]+raw.bytes[3]))
    return bad_checksum;

  memcpy(data, &raw.data.inner, sizeof(dht11_data));
  return success;
}
