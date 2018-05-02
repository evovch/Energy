/**************************************************************************
Ovcharenko Egor, jan 2016
evovch@gmail.com
**************************************************************************/

#include <sys/socket.h> // for all socket operations - socket, bind, etc...
#include <arpa/inet.h> // for htons
#include <unistd.h> // for close
#include <stdio.h> // for perror, (printf, fprintf, etc...)
#include <string.h> // for memset

#include "myPacket.hpp" // header with network frame description and serialization

#include <random> // C++ random number generator according to some distribution
#include <functional>

// For Ctrl+C signal handling
#include <signal.h>
volatile sig_atomic_t gStop;
void inthand(int /*signum*/) { gStop = 1; printf ("Ctrl+C signal caught.\n"); }

int main(int argc, char** argv)
{
   // Prepare client IP adress and port
   if (argc != 3) {
      fprintf(stderr, "Aborting. Client IP adress and/or port not specified.\n"
                      "Usage: ./server.exe <client IP> <client port>\n");
      return 1;
   }

   struct sockaddr_in sckAddr2; // other
   unsigned int sckAddr2len = sizeof(sckAddr2);

   // First zero then fill the structure
   memset((char *) &sckAddr2, 0, sckAddr2len);
   sckAddr2.sin_family = AF_INET;
   // port from program argument 2
   sckAddr2.sin_port = htons(atoi(argv[2]));

   int inetPtonRes;
   // ip adress from program argument 1
   inetPtonRes = inet_pton(AF_INET, argv[1], &(sckAddr2.sin_addr));
   if (inetPtonRes == -1) {
      perror("Aborting. Failed to run inet_pton method");
      return 1;
   }

   // Create a socket
   int s;
   s = socket(AF_INET, SOCK_DGRAM, 0);
   if (s == -1) {
      perror("Aborting. Failed to create socket");
      return 1;
   }

// ---------------------------------------------------------------------------

   // Connect signal handler to the signal Ctrl+C
   signal(SIGINT, inthand);

   // Counter of packets sent
   unsigned int counterOfPackets = 0;

   // Initialize random number factory
   std::default_random_engine generator;
   std::uniform_int_distribution<int> distribution(0,99);
   auto dice = std::bind ( distribution, generator );

   while (!gStop)
   {
      // Prepare data
      struct myPacket myPack;
      myPack.packetID = counterOfPackets;
      myPack.packetData = dice();
      //mySerialization::Dump(&myPack);
      //mySerialization::Print(&myPack);
      unsigned char buffer[8];
      mySerialization::SerializeMyPacket(buffer, &myPack);

      // Send data
      int sendToRes;
      sendToRes = sendto(s, buffer, 8, 0, (struct sockaddr*)&sckAddr2, sckAddr2len);
      if (sendToRes == -1) {
         perror("Aborting. Failed to send data");
         return 1; // return from main
      } else {
         printf ("Sent %d bytes.\n", sendToRes);
      }

      counterOfPackets++;

      usleep (1000000);
   }

   // Prepare a terminator
   struct myPacket termPack;
   termPack.packetID = 0xffffffff;
   termPack.packetData = 0xffffffff;
   unsigned char buffer[8];
   mySerialization::SerializeMyPacket(buffer, &termPack);
   // Send a terminator
   int sendToRes;
   sendToRes = sendto(s, buffer, 8, 0, (struct sockaddr*)&sckAddr2, sckAddr2len);
   if (sendToRes == -1) {
      perror("Failed to send terminator");
      //return 1; // return from main
   } else {
      printf ("Sent %d bytes terminator.\n", sendToRes);
   }

// ---------------------------------------------------------------------------

   // Finalize
   int closeRes;
   closeRes = close(s); // close socket
   if (closeRes == -1) {
      perror("Aborting. Failed to close socket");
      return 1; // return from main
   }

   return 0; // return from main
}
