/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include "contiki.h"

#if PLATFORM_HAS_BATTERY

#include <string.h>
#include "rest-engine.h"
#include "dev/battery-sensor.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

/* A simple getter example. Returns the reading from light sensor with a simple etag */
PERIODIC_RESOURCE(res_battery_obs,
                  "title=\"Battery status\";obs",
                  res_get_handler,
                  NULL,
                  NULL,
                  NULL,
                  30 * CLOCK_SECOND,
                  res_periodic_handler);

static int pushed_value = 0;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    REST.set_header_max_age(response, res_battery_obs.periodic->period / CLOCK_SECOND);
    REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "%d.%02d", pushed_value/1000, pushed_value % 1000));
}

static void
res_periodic_handler()
{
    
    PRINTF("periodic handler battery value \n");
    
    int battery = battery_sensor.value(0);
    
    PRINTF("battery=%d \n", battery);
    
    
    /* Usually a condition is defined under with subscribers are notified, e.g., large enough delta in sensor reading. */
    if (((battery + 10) <= pushed_value) || (battery >= (pushed_value + 10)) ) {
        
        pushed_value = battery;
        PRINTF("push battery value %u \n", pushed_value);
        
        /* Notify the registered observers which will trigger the res_get_handler to create the response. */
        REST.notify_subscribers(&res_battery_obs);
    }
}

#endif /* PLATFORM_HAS_BATTERY */
