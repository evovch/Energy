/**************************************************************************
Ovcharenko Egor, jan 2016
evovch@gmail.com
**************************************************************************/

// Test structure - network frame with two integers
struct myPacket
{
   unsigned int packetID;
   unsigned int packetData;
};

namespace mySerialization
{
   // Serialize a structure into the buffer of chars (bytes)
   unsigned char* SerializeMyPacket(unsigned char* buffer, struct myPacket* objToSerialize)
   {
      buffer[0] = (objToSerialize->packetID >> 24) & 0xFF;
      buffer[1] = (objToSerialize->packetID >> 16) & 0xFF;
      buffer[2] = (objToSerialize->packetID >> 8) & 0xFF;
      buffer[3] = (objToSerialize->packetID >> 0) & 0xFF;

      buffer[4] = (objToSerialize->packetData >> 24) & 0xFF;
      buffer[5] = (objToSerialize->packetData >> 16) & 0xFF;
      buffer[6] = (objToSerialize->packetData >> 8) & 0xFF;
      buffer[7] = (objToSerialize->packetData >> 0) & 0xFF;

      return buffer;
   }

   // Serialize the data from the buffer of chars (bytes) into the structure
   struct myPacket* DeserializePacket(unsigned char* buffer, struct myPacket* objToWriteIn)
   {
      objToWriteIn->packetID = ((buffer[0] << 24) & 0xFF000000) |
                               ((buffer[1] << 16) & 0x00FF0000) |
                               ((buffer[2] << 8)  & 0x0000FF00) |
                               ((buffer[3] << 0)  & 0x000000FF);

      objToWriteIn->packetData = ((buffer[4] << 24) & 0xFF000000) |
                                 ((buffer[5] << 16) & 0x00FF0000) |
                                 ((buffer[6] << 8)  & 0x0000FF00) |
                                 ((buffer[7] << 0)  & 0x000000FF);

      return objToWriteIn;
   }

   // Print raw data from the structure into the standart output
   void Dump(struct myPacket* objToDump)
   {
      unsigned char buffer[8];
      SerializeMyPacket(buffer, objToDump);
      for (unsigned int i=0; i<8; i++) {
         printf ("%02X", buffer[i]);
      }
      printf ("\n");
   }

   // Print readable data from the structure into the standart output
   void Print(struct myPacket* objToDump)
   {
      printf ("Packet: %p\n", objToDump);
      printf ("\tpacketID:   %u\n", objToDump->packetID);
      printf ("\tpacketData: %u\n", objToDump->packetData);
   }
}

