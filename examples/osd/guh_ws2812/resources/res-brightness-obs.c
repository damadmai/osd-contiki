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
 *      WS2812 set max count of leds
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Bernhard Trinnes <bernhard.trinnes@guh.guru>
 */

#include "contiki.h"
#include <string.h>
#include <stdlib.h>
#include "rest-engine.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


extern uint8_t effectbrightness;

static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
PERIODIC_RESOURCE(res_brightness_obs,
                  "title=\"brightness: percent=0-100\";obs",
                  res_get_handler,
                  res_post_put_handler,
                  res_post_put_handler,
                  NULL,
                  60 * CLOCK_SECOND,
                  res_periodic_handler);

static int pushed_value = 0;


static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    pushed_value = effectbrightness;
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    REST.set_header_max_age(response, res_brightness_obs.periodic->period / CLOCK_SECOND);
    REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "%d", pushed_value));
}


static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    const char *value = NULL;
    size_t len = 0;
    uint8_t success = 1;
    uint8_t tmp;
    
    
    if ((len = REST.get_post_variable(request, "percent", &value))){
        
        tmp = (uint8_t)atoi(value);
        PRINTF("Brightness: %u \n", tmp);
        if (tmp <= 100){
            effectbrightness = tmp;
        }else {
            PRINTF("Not allowed brightness value\n");
            success = 0;
        }
        
    } else {
        PRINTF("no mode post variable\n");
        success = 0;
    }
    if(!success) {
        REST.set_response_status(response, REST.status.BAD_REQUEST);
    }
}


static void
res_periodic_handler()
{
    
    PRINTF("periodic handler effect mode\n");
    
    /* Usually a condition is defined under with subscribers are notified, e.g., large enough delta in sensor reading. */
    if (effectbrightness != pushed_value) {
        
        PRINTF("push value %u \n", pushed_value);
        
        /* Notify the registered observers which will trigger the res_get_handler to create the response. */
        REST.notify_subscribers(&res_brightness_obs);
    }
}



