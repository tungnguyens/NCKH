#ifndef MY_INCLUDE_H
#define MY_INCLUDE_H

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#if USING_CC2530 == 1
#include "dev/cc2530-rf.h"
#include "adc.h"
#endif // USING_CC2530

#include "dev/watchdog.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/uip-udp-packet.h"
#include "net/uip-ds6-route.h"
#include "net/rpl/rpl.h"

#include "lib/random.h"

#include "debug.h"
#include "net/uip-debug.h"

#endif