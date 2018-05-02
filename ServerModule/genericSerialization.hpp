/**************************************************************************
Ovcharenko Egor, jan 2016
evovch@gmail.com
**************************************************************************/

/*
 Methods to pack integers of different size into the buffer of bytes (chars) and back.
*/

namespace genericSerialization
{
   // Returns the pointer to the next byte
   unsigned char* Serialize_uint32(unsigned char* buffer, uint32_t numToSerialize)
   {
      buffer[0] = (numToSerialize >> 24) & 0xFF;
      buffer[1] = (numToSerialize >> 16) & 0xFF;
      buffer[2] = (numToSerialize >> 8)  & 0xFF;
      buffer[3] = (numToSerialize >> 0)  & 0xFF;
      return buffer+4;
   }

   // Returns the pointer to the next byte
   unsigned char* Serialize_uint16(unsigned char* buffer, uint16_t numToSerialize)
   {
      buffer[0] = (numToSerialize >> 8)  & 0xFF;
      buffer[1] = (numToSerialize >> 0)  & 0xFF;
      return buffer+2;
   }

   // Returns the pointer to the next byte
   unsigned char* Serialize_uint8(unsigned char* buffer, uint8_t numToSerialize)
   {
      buffer[0] = (numToSerialize >> 0)  & 0xFF;
      return buffer+1;
   }

   uint32_t Deserialize_uint32(unsigned char* buffer)
   {
      return ((buffer[0] << 24) & 0xFF000000) |
             ((buffer[1] << 16) & 0x00FF0000) |
             ((buffer[2] << 8)  & 0x0000FF00) |
             ((buffer[3] << 0)  & 0x000000FF);  
   }

   uint16_t Deserialize_uint16(unsigned char* buffer)
   {
      return ((buffer[0] << 8)  & 0xFF00) |
             ((buffer[1] << 0)  & 0x00FF);  
   }

   uint8_t Deserialize_uint8(unsigned char* buffer)
   {
      return ((buffer[0] << 0)  & 0xFF);  
   }
}
