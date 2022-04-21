#line 1 "/home/nunu/Arduino/scdtr_final_pedro/msg_queue.cpp"
#include "msg_queue.h"

#include <Arduino.h>

etl::function_fv<&noInterrupts> lock;
etl::function_fv<&interrupts> unlock;

etl::queue_spsc_locked<etl::vector<uint8_t, MSG_SIZE>, MSG_QUEUE_SIZE> msg_queue(lock, unlock);