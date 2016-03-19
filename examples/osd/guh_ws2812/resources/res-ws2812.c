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
#include <string.h>
#include <stdlib.h>
#include "rest-engine.h"
#include "light_ws2812.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif



struct cRGB led[10];

static void res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(res_ws2812,
         "title=\"LEDs: ?number=0|1, POST/PUT color=r|g|b\";rt=\"Control\"",
         NULL,
         res_post_put_handler,
         res_post_put_handler,
         NULL);

static void
res_post_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  size_t len = 0;
  const char *color = NULL;
  const char *number = NULL;
  //uint8_t mode = 0;
  uint8_t pos = 0;
  char red[3];
  char green[3];
  char blue[3];
  uint8_t success = 1;

  if((len = REST.get_query_variable(request, "number", &number))) {
    //parsing which LED should be Addressed
    pos=(uint8_t)atoi(number);
    pos *= 2;
    PRINTF("pos: %u\n", pos);
  } else {    
    PRINTF("no number query variable\n");
    success = 0;
  }


  if((len = REST.get_post_variable(request, "color", &color))) {

    // color is a 3 digit hex string r|g|b
    // extract the hex values
    PRINTF("color: %s\n", color);
    strncpy(red, color, 2);
    strncpy(green, color+2, 2);
    strncpy(blue, color+4, 2);
    
    //add NULL at the end so strol can convert this string
    red[2] = '\0';
    green[2] = '\0';
    blue[2] = '\0';
    PRINTF("red: %s\n", red);
    PRINTF("green: %s\n", green);
    PRINTF("blue: %s\n", blue);
    //convert from hex to decimal
    
    led[pos].r = (uint8_t)strtol(red,NULL,16);
    led[pos].g = (uint8_t)strtol(green,NULL,16);
    led[pos].b = (uint8_t)strtol(blue,NULL,16);

    led[pos+1].r = led[pos].r;
    led[pos+1].g = led[pos].g;
    led[pos+1].b = led[pos].b;


    PRINTF("red: %u\n", led[pos].r);
    PRINTF("green: %u\n", led[pos].g);
    PRINTF("blue: %u\n", led[pos].b);
    

    //send the new array
    ws2812_sendarray((uint8_t *)led,10*3);
  } else {
    PRINTF("no color post variable\n");
    success = 0;
  }

/*
  if (success &&  (len=REST.get_post_variable(request, "mode", &value))) {
       mode = (uint8_t)atoi(value); // ToDo
  } else {
       success = 0;
  }
*/

  if(!success) {
    REST.set_response_status(response, REST.status.BAD_REQUEST);
  }
}

