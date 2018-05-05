/**************************************************************************
Ovcharenko Egor, jan 2016
evovch@gmail.com
**************************************************************************/

#include <sys/socket.h> // for all socket operations - socket, bind, etc...
#include <arpa/inet.h> // for htons
#include <unistd.h> // for close
#include <stdio.h> // for perror, (printf, fprintf, etc...)
#include <string.h> // for memset

#include "pc_packet.hpp" // header with network frame description and serialization

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
   // Prepare IP adress and port
   if (argc != 5) {
      fprintf(stderr, "Aborting. Wrong number of arguments\n"
                      "Usage: ./client_real.exe <IP adress> <port> <IP adress> <port>\n");
      return 1;
   }

   // ME --------------------------------------------------------------
   struct sockaddr_in sckAddr1; // me
   unsigned int sckAddr1len = sizeof(sckAddr1);

   // First zero then fill the structure
   memset((char *) &sckAddr1, 0, sckAddr1len);
   sckAddr1.sin_family = AF_INET;
   sckAddr1.sin_port = htons(atoi(argv[2])); // atoi(argv[2]) or 42001
   sckAddr1.sin_addr.s_addr = htonl(INADDR_ANY);
/*
   int inetPtonRes1;
   // (..., argv[1], ...) or "10.200.23.222"
   inetPtonRes1 = inet_pton(AF_INET, argv[1], &(sckAddr1.sin_addr));
   if (inetPtonRes1 == -1) {
      perror("Aborting. Failed to run inet_pton method for ME");
      return 1;
   }
*/
/*
   // OTHER -----------------------------------------------------------
   struct sockaddr_in sckAddr2; // other
   unsigned int sckAddr2len = sizeof(sckAddr2);

   // First zero then fill the structure
   memset((char *) &sckAddr2, 0, sckAddr2len);
   sckAddr2.sin_family = AF_INET;
   sckAddr2.sin_port = htons(atoi(argv[4])); // atoi(argv[4]) or 42000

   int inetPtonRes2;
   // (..., argv[3], ...) or "10.200.22.222"
   inetPtonRes2 = inet_pton(AF_INET, argv[3], &(sckAddr2.sin_addr));
   if (inetPtonRes2 == -1) {
      perror("Aborting. Failed to run inet_pton method for OTHER");
      return 1;
   }

   // -----------------------------------------------------------------
*/
   // Create a socket
   int s;
   s = socket(AF_INET, SOCK_DGRAM, 0);
   if (s == -1) {
      perror("Aborting. Failed to create socket");
      return 1;
   } else {
      //printf("Socket created successfully.\n");
   }

   // Bind
   int bindRes;
   bindRes = bind(s, (struct sockaddr*)&sckAddr1, sckAddr1len);
   if (bindRes == -1) {
      perror("Aborting. Failed to bind socket");
      return 1;
   } else {
      //printf("Socket bind successfully.\n");
   }

/*
   // Connect
   int connectRes;
   connectRes = connect(s, (struct sockaddr*)&sckAddr2, sckAddr2len);
   if (connectRes == -1) {
      perror("Aborting. Failed to connect socket");
      return 1;
   } else {
      //printf("Socket connected successfully.\n");
   }
*/

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

   // Recorded packet - only two integers
   struct rec_packet recordedPack;
   TFile* dataFile = new TFile("data.root", "RECREATE");
   TTree* dataTree = new TTree("dataTree", "dataTree");
   dataTree->Branch("recordedData", &recordedPack.second_count, "second_count/i:pc_counter_1");

   // Received packet - all 76 bytes
   struct pc_packet receivedPack;

   while (!gStop) // use 'while (1)' or 'while (!gStop)' for infinite loop with signal handling
   {
      alarm(5); // Set the timeout for recvfrom

      struct sockaddr_in sckAddr3;
      unsigned int sckAddr3len = sizeof(sckAddr3);

      unsigned char buffer[76];
      int recvFromRes;
      recvFromRes = recvfrom(s, buffer, 76, 0, (struct sockaddr*)&sckAddr3, &sckAddr3len);

      if (recvFromRes == -1) {
         printf ("Error receiving data. Terminating.\n");
         gStop = 1;
      } else {
         printf ("Received: %d bytes.\n", recvFromRes);
         Deserialize_pc_packet(buffer, &receivedPack);

         alarm(0); // Reset the timeout as the data was received

         // Copy required data from received packet into recorded packet
         recordedPack.second_count = receivedPack.second_count;
         recordedPack.pc_counter_1 = receivedPack.pc_counter_1;

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
