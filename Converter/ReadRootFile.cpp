/**************************************************************************
Ovcharenko Egor, march 2016
evovch@gmail.com
**************************************************************************/

// ROOT classes
#include <TApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>

struct data_from_file
{
	unsigned int col1;
	unsigned int col2;
	unsigned int col3;
	unsigned int col5;
};

int main(int argc, char** argv)
{
	if (argc != 2) {
		printf ("Incorrect number of arguments. Usage: ./readRootFile.exe <input root file>\n");
		return 1;
	}

	TApplication theApp("Analysis", &argc, argv);

	// Get the file
	TFile* dataFile = new TFile(theApp.Argv(1), "READ");

	// Get the only tree of the file
	TTree* dataTree = (TTree*)dataFile->Get("dataTree");

	// Get the only branch of the tree and map the branch to the structure
	struct data_from_file dataObject;
	TBranch* recDataBranch = dataTree->GetBranch("recordedData");
	recDataBranch->SetAddress(&dataObject);

	// Get the number of entries in the tree
	unsigned int numOfEntries = dataTree->GetEntries();

	// Search for min and max values
	//TODO optimize - there must be a dedicated method for that
	unsigned int col1_min=0, col1_max=1;
	unsigned int col2_min=0, col2_max=1;
	unsigned int col3_min=0, col3_max=1;
	unsigned int col5_min=0, col5_max=1;

	for (unsigned int i=0; i<numOfEntries; i++) {
		recDataBranch->GetEntry(i);
		if (dataObject.col1 < col1_min) col1_min = dataObject.col1;
		if (dataObject.col1 > col1_max) col1_max = dataObject.col1;
		if (dataObject.col2 < col2_min) col2_min = dataObject.col2;
		if (dataObject.col2 > col2_max) col2_max = dataObject.col2;
		if (dataObject.col3 < col3_min) col3_min = dataObject.col3;
		if (dataObject.col3 > col3_max) col3_max = dataObject.col3;
		if (dataObject.col5 < col5_min) col5_min = dataObject.col5;
		if (dataObject.col5 > col5_max) col5_max = dataObject.col5;
	}

	// Prepare histograms
	TH1D* histo1 = new TH1D("col1", "col1;;Entries", 1000, col1_min, col1_max);
	TH1D* histo2 = new TH1D("col2", "col2;;Entries", 1000, col2_min, col2_max);
	TH1D* histo3 = new TH1D("col3", "col3;;Entries", 1000, col3_min, col3_max);
	TH1D* histo5 = new TH1D("col5", "col5;;Entries", 1000, col5_min, col5_max);

	for (unsigned int i=0; i<numOfEntries; i++) {
		recDataBranch->GetEntry(i);
		histo1->Fill(dataObject.col1);
		histo2->Fill(dataObject.col2);
		histo3->Fill(dataObject.col3);
		histo5->Fill(dataObject.col5);
	}

	TCanvas* canv = new TCanvas("canv", "canv");
	canv->Divide(2, 2);

	canv->cd(1);	histo1->Draw();
	canv->cd(2);	histo2->Draw();
	canv->cd(3);	histo3->Draw();
	canv->cd(4);	histo5->Draw();

	theApp.Run();

	return 0;
}
