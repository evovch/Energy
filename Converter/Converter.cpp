/**************************************************************************
Ovcharenko Egor, march 2016
evovch@gmail.com
**************************************************************************/

// ROOT classes
#include <TFile.h>
#include <TTree.h>

struct data_from_file
{
	unsigned int col1;
	unsigned int col2;
	unsigned int col3;
	unsigned int col5;
};

int main(int argc, char** argv)
{
	if (argc != 3) {
		printf ("Incorrect number of arguments. Usage: ./converter.exe <input text file> <output root file>\n");
		return 1;
	}

	// Open the input text file
	FILE* inputTextFile = fopen(argv[1], "r");

	if (inputTextFile == NULL) {
		printf ("Error opening input file '%s'\n", argv[1]);
		return 1;
	} else {
		printf ("Succesfully opened input file '%s'\n", argv[1]);
	}

	// Create the output root file, a tree inside and a branch
	struct data_from_file dataObject;
	TFile* dataFile = new TFile(argv[2], "RECREATE");
	TTree* dataTree = new TTree("dataTree", "dataTree");
	dataTree->Branch("recordedData", &dataObject.col1, "col1/i:col2:col3:col5");

	// Declare temporary containers
	unsigned int col1, col2, col3, col5;           // 4
	unsigned int date_year, date_month, date_day;  // 3
	unsigned int time_h, time_m, time_s, time_ms;  // 4

	// Read the input file line by line,
	// loop {store the read information in the structure, fill the tree},
	// count the number of entries read
	unsigned int readLines = 0;
	while (fscanf(inputTextFile, "%d\t%d\t%d\t%d-%d-%d %d:%d:%d.%d\t%d",
	       &col1, &col2, &col3, &date_year, &date_month, &date_day, &time_h, &time_m, &time_s, &time_ms, &col5) == 11)
	{
		dataObject.col1 = col1;
		dataObject.col2 = col2;
		dataObject.col3 = col3;
		dataObject.col5 = col5;
		dataTree->Fill();
		readLines++;
	}
	printf ("Read %d entries\nCreated output file '%s'\n", readLines, argv[2]);

	// Write output ROOT file with received data and close it
	dataFile->Write();
	dataFile->Close();

	return 0; // return from main
}
