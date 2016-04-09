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
 *      button sensor for grove sensor on a merkur board
 * \author
 *      Bernhard Trinnes <bernhard.trinnes@guh.guru>
 */



/*
 * For button grove module
 * Connected to J1 - D8 - PD0 - INT0
 */


/* Sensor routine */
#include "contiki.h"
#include "lib/sensors.h"
#include "button-sensor.h"
#include <avr/interrupt.h>




const struct sensors_sensor button_sensor;

static struct timer debouncetimer;
static int status(int type);
static int enabled = 0;
struct sensors_sensor *sensors[1];
unsigned char sensors_flags[1];

#define BUTTON_BIT INTF0
#define BUTTON_CHECK_IRQ() (EIFR & BUTTON_BIT) ? 0 : 1

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
ISR(INT0_vect)
{
    PRINTF("INTO interrupt triggered \n");
    if(BUTTON_CHECK_IRQ()) {
        PRINTF("button check \n");
        if(timer_expired(&debouncetimer)) {
            timer_set(&debouncetimer, CLOCK_SECOND / 4);
            sensors_changed(&button_sensor);
            PRINTF("Sensor changed \n");

        }
    }
    
}
/*---------------------------------------------------------------------------*/

static int
value(int type)
{
    return (PORTD & _BV(PD0) ? 0 : 1) || !timer_expired(&debouncetimer);
    //return 0;
}

static int
configure(int type, int c)
{
    PRINTF("Configuring Interrupt INT0 \n");
    switch (type) {
        case SENSORS_ACTIVE:
            if (c) {
                if(!status(SENSORS_ACTIVE)) {
                    timer_set(&debouncetimer, 0);
                    DDRD  |= (0<<DDD0); // Set pin as input
                    PORTD |= (1<<PORTD0); // Set port PORTE bint 6 with pullup resistor
                    EICRA |= (3<<ISC00); // For rising edge
                    EIMSK |= (1<<INT0); // Set int
                    enabled = 1;
                    sei();
                    PRINTF("Interrupt active \n");
                }
            } else {
                enabled = 0;
                EIMSK &= ~(1<<INT0); // clear int
            }
            return 1;
    }
    return 0;
}

static int
status(int type)
{
    PRINTF("status interrupt %d \n", type);
    switch (type) {
        case SENSORS_ACTIVE:
        case SENSORS_READY:
            return enabled;//(EIMSK & (1<<INT6) ? 0 : 1);//BUTTON_IRQ_ENABLED();
    }
    return 0;
}

SENSORS_SENSOR(button_sensor, BUTTON_SENSOR,
               value, configure, status);

