#line 1 "/home/nunu/Arduino/scdtr_final_pedro/protocol.h"
#pragma once

// <sender ID> <msg size> <CMD> <data> ... <data>

#define BROADCAST 0x00

#define WAKEUP 'W'
#define MEASURE 'M'
#define CALIBRATE 'C'
#define DONE_CALIBRATE 'D'
#define MEASURE_ACK 'm'


