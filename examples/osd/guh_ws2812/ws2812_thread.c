#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "light_ws2812.h"


#include "contiki.h"

PROCESS(ws2812_process, "Example process");
AUTOSTART_PROCESSES(&ws2812_process);

static struct etimer timer;

PROCESS_THREAD(ws2812_process, ev, data)
{
    PROCESS_BEGIN();
    
    while(1) {
        
        etimer_set(&timer, CLOCK_SECOND/25);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        
        
    }
    
    PROCESS_END();
}