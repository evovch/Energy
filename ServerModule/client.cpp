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

//#include "pc_packet.hpp"

// ROOT classes
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>

// For Ctrl+C signal handling
#include <signal.h>
volatile sig_atomic_t gStop;
void inthand(int /*signum*/) { gStop = 1; printf ("Ctrl+C signal caught.\n"); }

void CatchAlarm(int /*ignored*/)
{
   printf ("Aborting. No data for 5 seconds.\n");
}

int main(int argc, char** argv)
{
   struct sockaddr_in sckAddr1; // me
   unsigned int sckAddr1len = sizeof(sckAddr1);

   // First zero then fill the structure
   memset((char *) &sckAddr1, 0, sckAddr1len);
   sckAddr1.sin_family = AF_INET;
   sckAddr1.sin_port = htons(42001);
   sckAddr1.sin_addr.s_addr = htonl(INADDR_ANY);

   // Create a socket
   int s;
   s = socket(AF_INET, SOCK_DGRAM, 0);
   if (s == -1) {
      perror("Aborting. Failed to create socket");
      return 1;
   }

   // Bind socket and structure
   int bindRes;
   bindRes = bind(s, (struct sockaddr*)&sckAddr1, sckAddr1len);
   if (bindRes == -1) {
      perror("Aborting. Failed to bind socket");
      return 1;
   }

// ---------------------------------------------------------------------------

   // Connect signal handler to the signal Ctrl+C
   signal(SIGINT, inthand);

   /* Set signal handler for alarm signal */
   struct sigaction myAction;
   myAction.sa_handler = CatchAlarm;
   if (sigfillset(&myAction.sa_mask) < 0) {
      perror("Aborting. sigfillset failed");
      return 1;
   }
   myAction.sa_flags = 0;

   int sigActRes;
   sigActRes = sigaction(SIGALRM, &myAction, 0);
   if (sigActRes == -1) {
      perror("Aborting. Sigaction failed");
      return 1;
   }

   // Create a special ROOT application singleton
   TApplication theApp("Analysis", &argc, argv);

   struct myPacket receivedPacket;
   TFile* dataFile = new TFile("data.root", "RECREATE");
   TTree* dataTree = new TTree("dataTree", "dataTree");
   dataTree->Branch("receivedData", &receivedPacket.packetID, "packetID/i:packetData");

   struct sockaddr_in sckAddr2; // other
   unsigned int sckAddr2len = sizeof(sckAddr2);

   while (!gStop) // use 'while (1)' or 'while (!gStop)' for infinite loop with signal handling
   {
      alarm(5); // Set the timeout for recvfrom

      unsigned char buffer[8];
      int recvFromRes;
      recvFromRes = recvfrom(s, buffer, 8, 0, (struct sockaddr*)&sckAddr2, &sckAddr2len);

      if (recvFromRes == -1) {
         printf ("Error receiving data. Terminating.\n");
         gStop = 1;
      } else {
         printf ("Received: %d bytes.\n", recvFromRes);
         mySerialization::DeserializePacket(buffer, &receivedPacket);
         //mySerialization::Dump(&receivedPacket);
         //mySerialization::Print(&receivedPacket);

         alarm(0); // Reset the timeout as the data was received

         // Check if the received packet is a terminator
         if (receivedPacket.packetID == 0xffffffff) break;

         // save the data
         dataTree->Fill();
      }

   }

   // Write output ROOT file with received data
   dataFile->Write();

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
