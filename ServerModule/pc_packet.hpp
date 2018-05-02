/**************************************************************************
Ovcharenko Egor, jan 2016
evovch@gmail.com
**************************************************************************/

#include "genericSerialization.hpp"

// Structure for the data to be written to disk
struct rec_packet
{
   uint32_t second_count;
   uint32_t pc_counter_1;
};

// Structure for the received data
// 76 bytes
struct pc_packet
{
   uint32_t ip_addr;
   uint32_t ip_mask;
   uint32_t ip_gw;
   uint32_t ip_server;
   uint8_t mac_addr[6];
   uint8_t host_name[16];
   uint8_t mode_work;
   uint8_t E_freq_max;
   uint8_t E_freq_min;
   uint8_t E_freq_work;
   uint8_t manag_key;
   uint8_t manag_freq;
   uint8_t manag_status;
   uint16_t pointer_mode;
   uint32_t second_count;
   uint32_t tick_count;
   uint32_t pc_counter_1;
   uint32_t pc_counter_2;
   uint32_t pc_counter_3;
   uint32_t ttt6;
   uint16_t ttt7;
   uint8_t priznak;
   uint16_t cksum;
};

struct pc_packet* Deserialize_pc_packet(unsigned char* buffer, struct pc_packet* packToSerialize)
{
   unsigned int cursor = 0;
   packToSerialize->ip_addr = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->ip_mask = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->ip_gw = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->ip_server = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   for (unsigned int i=0; i<6; i++) {
     packToSerialize->mac_addr[i] = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   }
   for (unsigned int i=0; i<16; i++) {
     packToSerialize->host_name[i] = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   }
   packToSerialize->mode_work = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->E_freq_max = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->E_freq_min = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->E_freq_work = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->manag_key = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->manag_freq = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->manag_status = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->pointer_mode = genericSerialization::Deserialize_uint16(&buffer[cursor]); cursor+=2;
   packToSerialize->second_count = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->tick_count = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->pc_counter_1 = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->pc_counter_2 = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->pc_counter_3 = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->ttt6 = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=4;
   packToSerialize->ttt7 = genericSerialization::Deserialize_uint32(&buffer[cursor]); cursor+=2;
   packToSerialize->priznak = genericSerialization::Deserialize_uint8(&buffer[cursor]); cursor+=1;
   packToSerialize->cksum = genericSerialization::Deserialize_uint16(&buffer[cursor]); cursor+=2;

   return packToSerialize;
}

// Return pointer to the next byte in the buffer
unsigned char* Serialize_pc_packet(unsigned char* buffer, struct pc_packet packToSerialize)
{
   unsigned char* cursor = buffer;

   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.ip_addr);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.ip_mask);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.ip_gw);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.ip_server);
   for (unsigned int i=0; i<6; i++) {
      cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.mac_addr[i]);
   }
   for (unsigned int i=0; i<16; i++) {
      cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.host_name[i]);
   }
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.mode_work);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.E_freq_max);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.E_freq_min);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.E_freq_work);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.manag_key);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.manag_freq);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.manag_status);
   cursor = genericSerialization::Serialize_uint16(cursor, packToSerialize.pointer_mode);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.second_count);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.tick_count);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.pc_counter_1);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.pc_counter_2);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.pc_counter_3);
   cursor = genericSerialization::Serialize_uint32(cursor, packToSerialize.ttt6);
   cursor = genericSerialization::Serialize_uint16(cursor, packToSerialize.ttt7);
   cursor = genericSerialization::Serialize_uint8(cursor, packToSerialize.priznak);
   cursor = genericSerialization::Serialize_uint16(cursor, packToSerialize.cksum);

   return cursor;
}

//FIXME check print types
void Print_pc_packet(struct pc_packet* packToPrint)
{
   printf ("pc_packet: %p\n", packToPrint);
   printf ("\tip_addr:\t%u", packToPrint->ip_addr);
   printf ("\tip_mask:\t%u", packToPrint->ip_mask);
   printf ("\tip_gw:\t%u", packToPrint->ip_gw);
   printf ("\tip_server:\t%u", packToPrint->ip_server);
   for (unsigned int i=0; i<6; i++) {
      printf ("\tmac_addr[%d]:\t%u", i, packToPrint->mac_addr[i]);
   }
   for (unsigned int i=0; i<16; i++) {
      printf ("\thost_name[%d]:\t%u", i, packToPrint->host_name[i]);
   }
   printf ("\tmode_work:\t%u", packToPrint->mode_work);
   printf ("\tE_freq_max:\t%u", packToPrint->E_freq_max);
   printf ("\tE_freq_min:\t%u", packToPrint->E_freq_min);
   printf ("\tE_freq_work:\t%u", packToPrint->E_freq_work);
   printf ("\tmanag_key:\t%u", packToPrint->manag_key);
   printf ("\tmanag_freq:\t%u", packToPrint->manag_freq);
   printf ("\tmanag_status:\t%u", packToPrint->manag_status);
   printf ("\tpointer_mode:\t%u", packToPrint->pointer_mode);
   printf ("\tsecond_count:\t%u", packToPrint->second_count);
   printf ("\ttick_count:\t%u", packToPrint->tick_count);
   printf ("\tpc_counter_1:\t%u", packToPrint->pc_counter_1);
   printf ("\tpc_counter_2:\t%u", packToPrint->pc_counter_2);
   printf ("\tpc_counter_3:\t%u", packToPrint->pc_counter_3);
   printf ("\tttt6:\t%u", packToPrint->ttt6);
   printf ("\tttt7:\t%u", packToPrint->ttt7);
   printf ("\tpriznak:\t%u", packToPrint->priznak);
   printf ("\tcksum:\t%u", packToPrint->cksum);
}
