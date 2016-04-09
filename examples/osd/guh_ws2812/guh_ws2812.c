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
 *      Erbium (Er) REST Engine example.
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include <util/delay.h>
#include "light_ws2812.h"
#include "sys/rtimer.h"
#include "dev/battery-sensor.h"
#include "dev/leds.h"

#define     PERIOD_T     RTIMER_SECOND/25

#undef  RAND_MAX
#define RAND_MAX 0xef // defines max value for rand()


struct cRGB colors[8];
struct cRGB led[MAXPIX];

uint8_t effectmode;
uint8_t effectspeed;
uint8_t effectbrightness;
uint8_t maxpix = MAXPIX;
uint8_t colorlenght;
struct cRGB effectcolor;
struct cRGB tcolor[3];

//static struct ctimer timer;
static struct rtimer timer;

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */


static char periodic_rtimer(struct rtimer *rt, void* ptr){
    uint8_t ret;
    float brightness;
    uint16_t period;
    rtimer_clock_t time_now = RTIMER_NOW();
    
    
    static int i, j, k, up, tmp;
    float r, g, b;
    
    brightness = effectbrightness/100;
    period = (uint8_t)(RTIMER_SECOND/((25/240)*effectspeed)); //25 = max calles per second 25fps
    
    /*
     "0 - Off",
     "1 - Color On",
     "2 - Color Wave",
     "3 - Color Fade",
     "4 - Color Flash",
     "5 - Rainbow Wave",
     "6 - Rainbow Flash",
     "7 - Knight Rider",
     "8 - Fire"
     "9 - Tricolore"
     
     */
    if (effectmode != tmp) {
        // if effect changes, than reset all parameters
        i=0;
        j=0;
        up=1;
        tmp = effectmode;
    }
    
    
    
    PRINTF("Timer triggered \n effectmode: %u \n", effectmode);
    switch (effectmode) {
        case 0:
            /*---------------------------
             *
             *   EFFECT MODE OFF
             *
             *---------------------------*/
            for(i=maxpix; i>0; i--)
            {
                led[i-1].r = 0;
                led[i-1].g = 0;
                led[i-1].b = 0;
                
            }
            break;
            
        case 1:
            
            /*---------------------------
             *
             *   EFFECT MODE COLOR ON
             *
             *---------------------------*/
            for(i=maxpix; i>0; i--)
            {
                led[i-1].r = effectcolor.r*brightness;
                led[i-1].g = effectcolor.g*brightness;
                led[i-1].b = effectcolor.b*brightness;
            }

            break;
        case 2:
            /*---------------------------
             *
             *   EFFECT MODE COLOR WAVE
             *
             *---------------------------*/
            
            
            if (up) {
                j+=1;
                if (j == 16){
                    up = 0;
                }
            } else {
                j-=1;
                if (j == 0){
                    up = 1;
                }
            }
            
            r = ((j*j)/256.0)*(effectcolor.r*brightness);
            g = ((j*j)/256.0)*(effectcolor.g*brightness);
            b = ((j*j)/256.0)*(effectcolor.b*brightness);
            
            led[0].r = (uint8_t)r;
            led[0].g = (uint8_t)g;
            led[0].b = (uint8_t)b;
            
            
            for(i=maxpix; i>1; i--) // shift leds
                led[i-1]=led[i-2];
            
            /*
            
            
            if(led[0].r>(effectcolor.r+FADE))
                led[0].r-=FADE;
            
            if(led[0].g<(effectcolor.g-FADE))
                led[0].g+=FADE;
            
            if(led[0].g>(effectcolor.g+FADE))
                led[0].g-=FADE;
            
            if(led[0].b<(effectcolor.b-FADE))
                led[0].b+=FADE;
            
            if(led[0].b>(effectcolor.b+FADE))
                led[0].b-=FADE;
             */
            
            break;
            
        case 3:
            /*---------------------------
             *
             *   EFFECT MODE COLOR FADE
             *
             *---------------------------*/
            
            if (up) {
                j++;
                if (j == 255){
                    up = 0;
                }
            } else {
                j--;
                if (j == 0){
                    up = 1;
                }
            }
            
            r = (j/255.0)*effectcolor.r;
            g = (j/255.0)*effectcolor.g;
            b = (j/255.0)*effectcolor.b;
            
            led[0].r = (uint8_t)r;
            led[0].g = (uint8_t)g;
            led[0].b = (uint8_t)b;
            
            for(i=maxpix; i>1; i--) // shift leds
                led[i-1]=led[0];
            
            break;
            
            
        case 4:
            
            /*---------------------------
             *
             *   EFFECT MODE COLOR FLASH
             *
             *---------------------------*/
            
            if (up) {
                j++;
                if (j >= 5){
                    up = 0;
                    j = 0;
                }
                
                for(i=maxpix; i>0; i--)
                {
                    led[i-1] = effectcolor;
                    
                }
            }else{
                j++;
                if (j >= 5){
                    up = 1;
                    j = 0;
                }
                
                for(i=maxpix; i>0; i--)
                {
                    led[i-1].r = 0;
                    led[i-1].g = 0;
                    led[i-1].b = 0;
                }
            }
            break;
            
        case 5:
            /*---------------------------
            *
            *   EFFECT MODE RAINBOW WAVE
            *
            *---------------------------*/
             
             
            for(i=maxpix; i>1; i--)
                led[i-1]=led[i-2];
            //change colour when colourlength is reached
            if(k>COLORLENGTH){
                j++;
                if(j>7)
                {
                    j=0;
                }
                k=0;
            }
            k++;
            //loop colouers
            
            //fade red
            if(led[0].r<(colors[j].r-FADE))
                led[0].r+=FADE;
            
            if(led[0].r>(colors[j].r+FADE))
                led[0].r-=FADE;
            
            if(led[0].g<(colors[j].g-FADE))
                led[0].g+=FADE;
            
            if(led[0].g>(colors[j].g+FADE))
                led[0].g-=FADE;
            
            if(led[0].b<(colors[j].b-FADE))
                led[0].b+=FADE;
            
            if(led[0].b>(colors[j].b+FADE))
                led[0].b-=FADE;
            
            break;
            
        case 6:
            /*---------------------------
             *
             *   EFFECT MODE RAINBOW FLASH
             *
             *---------------------------*/
            
            if (up) {
                j++;
                if (j >= 7) {
                    up = 0;
                }
            } else {
                j--;
                if (j <= 1) {
                    up = 1;
                }
            }
            
            for(i=maxpix; i>0; i--)
            {
                led[i-1]=colors[j];
            }
            
            break;
            
            
        case 7:
            /*---------------------------
             *
             *   EFFECT MODE KNIGHT RIDER
             *
             *---------------------------*/
            
            for(i=maxpix; i>0; i--)
            {
                led[i-1].r = 10;
                led[i-1].g = 0;
                led[i-1].b = 0;
            }
            
            
            if (up) {
                j += 1;
                if (j>= (maxpix - 15)) {
                    up = 0;
                }
            } else{
                j-= 1;
                if (j <= 0) {
                    up = 1;
                }
            }
            
            led[j].r = 50;
            led[j+1].r = 50;
            led[j+2].r = 100;
            led[j+3].r = 100;
            led[j+4].r = 150;
            led[j+5].r = 180;
            led[j+6].r = 200;
            led[j+7].r = 255;
            led[j+8].r = 255;
            led[j+9].r = 200;
            led[j+10].r = 180;
            led[j+11].r = 150;
            led[j+12].r = 100;
            led[j+13].r = 80;
            led[j+14].r = 50;
            led[j+15].r = 30;
            
            break;
        case 8:
            /*---------------------------
             *
             *   EFFECT MODE FIRE EFFECT
             *
             *---------------------------*/
            for(i=(maxpix/12); i>0; i--)
            {
                j = (uint8_t)rand();
                j %= maxpix;
                led[j].r = 80;
                led[j].g = 35;
                led[j].b = 0;
            }
            
            for(i=(maxpix/12); i>0; i--)
            {
                j = (uint8_t)rand();
                j %= maxpix;
                led[j].r = 25;
                led[j].g = 8;
                led[j].b = 0;
            }
            for(i=(maxpix/12); i>0; i--)
            {
                j = (uint8_t)rand();
                j %= maxpix;
                led[j].r = 10;
                led[j].g = 3;
                led[j].b = 0;
            }
            for(i=(maxpix/12); i>0; i--)
            {
                j = (uint8_t)rand();
                j %= maxpix;
                led[j].r = 100;
                led[j].g = 35;
                led[j].b = 0;
            }
            break;
            
        case 9:
            /*---------------------------
             *
             *   EFFECT MODE TRICOLORE
             *
             *---------------------------*/
            
            
            // Color one 0 bis maxpix/3
            for(i=(maxpix/3); i>0; i--)
            {
                led[i-1].r=tcolor[0].r*brightness;
                led[i-1].g=tcolor[0].g*brightness;
                led[i-1].b=tcolor[0].b*brightness;
            }
            
            //Color two maxpix/3 bis 2 maxpix/3
            for(i=2*(maxpix/3); i>(maxpix/3); i--)
            {
                led[i-1].r=tcolor[1].r*brightness;
                led[i-1].g=tcolor[1].g*brightness;
                led[i-1].b=tcolor[1].b*brightness;
            }
            
            //Color three 2 maxpix/3 bis maxpix
            for(i=maxpix; i>(maxpix/3); i--)
            {
                led[i-1].r=tcolor[2].r*brightness;
                led[i-1].g=tcolor[2].g*brightness;
                led[i-1].b=tcolor[2].b*brightness;
            }
            
            break;
        default:
            effectmode = 0;
            break;
    }
    
    ws2812_sendarray((uint8_t *)led, maxpix*3);
    
    ret = rtimer_set(&timer, time_now + period, 1, (void (*)(struct rtimer *, void *)) periodic_rtimer, NULL);
    if(ret){
        printf("Error Timer: %u\n", ret);
    }
    return 1;
}

void
hw_init()
{
    leds_off(LEDS_RED);
    
    uint8_t i;
    for(i=maxpix; i>0; i--)
    {
        led[i-1].r=0;
        led[i-1].g=0;
        led[i-1].b=0;
    }
    ws2812_sendarray((uint8_t *)led,maxpix*3);
    
    //Rainbowcolors
    colors[0].r=150; colors[0].g=150; colors[0].b=150;
    colors[1].r=255; colors[1].g=000; colors[1].b=000;//red
    colors[2].r=255; colors[2].g=100; colors[2].b=000;//orange
    colors[3].r=100; colors[3].g=255; colors[3].b=000;//yellow
    colors[4].r=000; colors[4].g=255; colors[4].b=000;//green
    colors[5].r=000; colors[5].g=100; colors[5].b=255;//light blue (türkis)
    colors[6].r=000; colors[6].g=000; colors[6].b=255;//blue
    colors[7].r=100; colors[7].g=000; colors[7].b=255;//violet
    
}

PROCESS(er_example_server, "Erbium Example Server");
AUTOSTART_PROCESSES(&er_example_server);

PROCESS_THREAD(er_example_server, ev, data)
{
    PROCESS_BEGIN();
    
    PROCESS_PAUSE();
    
    PRINTF("Starting Erbium Example Server\n");
    
#ifdef RF_CHANNEL
    PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
    PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif
    
    PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
    PRINTF("LL header: %u\n", UIP_LLH_LEN);
    PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
    PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);
    
    hw_init();
    periodic_rtimer(&timer, NULL);
    
    DDRB|=_BV(ws2812_pin);
    
    
    /* Initialize the REST engine. */
    rest_init_engine();
    
    /*
     * Bind the resources to their Uri-Path.
     * WARNING: Activating twice only means alternate path, not two instances!
     * All static variables are the same for each URI path.
     */
    
    extern resource_t res_version;
    rest_activate_resource(&res_version, "p/version");
    
    extern resource_t res_maxpix_obs;
    rest_activate_resource(&res_maxpix_obs, "p/maxpix");
    
    extern resource_t res_effect_obs;
    rest_activate_resource(&res_effect_obs, "a/effect");
    
    extern resource_t res_color_obs;
    rest_activate_resource(&res_color_obs, "a/color");
    
    extern resource_t res_speed_obs;
    rest_activate_resource(&res_speed_obs, "a/speed");
    
    extern resource_t res_brightness_obs;
    rest_activate_resource(&res_brightness_obs, "a/brightness");
    
    extern resource_t res_tcolor_obs;
    rest_activate_resource(&res_tcolor_obs, "a/tcolor");
    
    extern resource_t res_singleled;
    rest_activate_resource(&res_singleled, "a/led");
    
    extern resource_t res_battery_obs;
    rest_activate_resource(&res_battery_obs, "s/battery");
    SENSORS_ACTIVATE(battery_sensor);
    
    
    
    /* Define application-specific events here. */
    while(1) {
        PROCESS_WAIT_EVENT();
        
    }                             /* while (1) */
    
    PROCESS_END();
}
