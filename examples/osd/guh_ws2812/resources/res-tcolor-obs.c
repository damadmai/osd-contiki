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

#include <string.h>
#include <stdlib.h>
#include "rest-engine.h"
#include "er-coap.h"
#include "light_ws2812.h"



#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

PERIODIC_RESOURCE(res_tcolor_obs,
                  "title=\"effect color\";obs",
                  res_get_handler,
                  res_post_put_handler,
                  res_post_put_handler,
                  NULL,
                  10 * CLOCK_SECOND,
                  res_periodic_handler);

/*
 * Use local resource state that is accessed by res_get_handler() and altered by res_periodic_handler() or PUT or POST.
 */

extern struct cRGB tcolor[3];
static struct cRGB pushed_value[3];

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    /*
     * For minimal complexity, request query and options should be ignored for GET on observable resources.
     * Otherwise the requests must be stored with the observer list and passed by REST.notify_subscribers().
     * This would be a TODO in the corresponding files in contiki/apps/erbium/!
     */
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
    REST.set_header_max_age(response, res_tcolor_obs.periodic->period / CLOCK_SECOND);
    REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "%0x%0x%0x%0x%0x%0x%0x%0x%0x", pushed_value[0].r, pushed_value[0].g, pushed_value[0].b, pushed_value[1].r, pushed_value[1].g, pushed_value[1].b,pushed_value[2].r, pushed_value[2].g, pushed_value[2].b));
    
    /* The REST.subscription_handler() will be called for observable resources by the REST framework. */
}



static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    size_t len = 0;
    const char *color = NULL;
    char red[3];
    char green[3];
    char blue[3];
    uint8_t success = 1, i;
    
    len = REST.get_post_variable(request, "color", &color);
    
    if(len) {
        
        PRINTF("color: %s\n", color);
        
        for (i=0; i<3; i++) {
            
            strncpy(red, color+(i*6), 2);
            strncpy(green, color+2+(i*6), 2);
            strncpy(blue, color+4+(i*6), 2);
            
            //add NULL at the end so strol can convert this string
            red[2] = '\0';
            green[2] = '\0';
            blue[2] = '\0';
            PRINTF("red: %s\n", red);
            PRINTF("green: %s\n", green);
            PRINTF("blue: %s\n", blue);
            //convert from hex to decimal
            
            tcolor[i].r = (uint8_t)strtol(red,NULL,16);
            tcolor[i].g = (uint8_t)strtol(green,NULL,16);
            tcolor[i].b = (uint8_t)strtol(blue,NULL,16);
            
            PRINTF("tcolor: %u %u %u \n", tcolor[i].r, tcolor[i].g, tcolor[i].b);
        }
        
    } else {
        PRINTF("no color post variable\n");
        success = 0;
    }
    
    
    if(!success) {
        REST.set_response_status(response, REST.status.BAD_REQUEST);
    }
}




/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
static void
res_periodic_handler()
{
    
    /* Do a periodic task here, e.g., sampling a sensor. */
    PRINTF("periodic effect color handler\n");
    
    /* Usually a condition is defined under with subscribers are notified, e.g., large enough delta in sensor reading. */
    if ((tcolor[0].r != pushed_value[0].r) || (tcolor[0].g != pushed_value[0].g) || (tcolor[0].b != pushed_value[0].b)) {
        
        pushed_value[0] = tcolor[0];
        /* Notify the registered observers which will trigger the res_get_handler to create the response. */
        REST.notify_subscribers(&res_tcolor_obs);
    }
}
