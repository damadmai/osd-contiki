/*
 * light_ws2812_config.h
 *
 * Created: 18.01.2014 09:58:15
 *
 * User Configuration file for the light_ws2812_lib
 *
 */ 


#ifndef WS2812_CONFIG_H_
#define WS2812_CONFIG_H_

///////////////////////////////////////////////////////////////////////
// Define I/O pin
///////////////////////////////////////////////////////////////////////

#define ws2812_port B     // Data port 
#define ws2812_pin  1     // Data out pin
#define MAXPIX 240
#define COLORLENGTH 40/2
#define FADE 256/COLORLENGTH

#endif /* WS2812_CONFIG_H_ */