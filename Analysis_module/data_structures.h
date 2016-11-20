#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_

#include <stdint.h>

struct rec_packet
{
   uint32_t second_count;
   uint32_t pc_counter_1;
   uint8_t tm_sec;
   uint8_t tm_min;
   uint8_t tm_hour;
   uint16_t tm_year; // year since 1900
   uint16_t tm_yday; // day since Jan 1 of this year
};


#endif // _DATA_STRUCTURES_H_
