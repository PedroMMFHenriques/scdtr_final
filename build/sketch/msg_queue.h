#line 1 "/home/nunu/Arduino/scdtr_final_pedro/msg_queue.h"
#pragma once

#include <Embedded_Template_Library.h>
#include <etl/vector.h>
#include <etl/queue_spsc_locked.h>

#include <stdint.h>
#include "settings.h"

extern etl::queue_spsc_locked<etl::vector<uint8_t, MSG_SIZE>, MSG_QUEUE_SIZE> msg_queue;