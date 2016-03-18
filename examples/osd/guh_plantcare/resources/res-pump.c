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
 *      Resource Pump for Plant Care
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 *      Bernhard Trinnes <bernhard.trinnes@guh.guru>
 */

#include "contiki.h"
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "Arduino.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);


RESOURCE(res_pump,
         "title=\"pump: POST/PUT mode=0|1 \";rt=\"Control\"",
         res_get_handler,
         res_post_put_handler,
         res_post_put_handler,
         NULL);

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    //get pump state
    uint8_t mode = digitalRead(7);
    
    unsigned int accept = -1;
    REST.get_header_accept(request, &accept);
    
    if(accept == -1 || accept == REST.type.TEXT_PLAIN) {
        REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", mode);
        
        REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
    } else if(accept == REST.type.APPLICATION_XML) {
        REST.set_header_content_type(response, REST.type.APPLICATION_XML);
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<State =\"%u\">", mode);
        
        REST.set_response_payload(response, buffer, strlen((char *)buffer));
    } else if(accept == REST.type.APPLICATION_JSON) {
        REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
        snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'pump':{'mode':%u}", mode);
        
        REST.set_response_payload(response, buffer, strlen((char *)buffer));
    } else {
        REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
        const char *msg = "Supporting content-types text/plain, application/xml, and application/json";
        REST.set_response_payload(response, msg, strlen(msg));
    }
}


static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    size_t len = 0;
    const char *mode = NULL;
    int success = 1;
    int a = -1;
    len = REST.get_post_variable(request, "mode", &mode);
    
    if(len) {
        PRINTF("mode %s\n", mode);
        a = atoi(mode);
        
        if(a == 0) {
            //pump off
            PRINTF("pump off\n");
            digitalWrite(7, LOW);
        } else if(a == 1) {
            //pump on
            PRINTF("mode on\n");
            digitalWrite(7, HIGH);
        } else {
            success = 0;
        }
    } else {
        success = 0;
    }
    
    if(!success) {
        REST.set_response_status(response, REST.status.BAD_REQUEST);
    }
}
