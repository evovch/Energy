void extractData(TString filename="data.root")
{
	TFile* inputFile = new TFile(filename);
	TTree* curTree = inputFile->Get("dataTree");
	TBranch* curBranch = curTree->GetBranch("receivedData");

	TCanvas* c1 = new TCanvas("c1", "c1");
	curTree->Draw("packetData");

	TCanvas* c2 = new TCanvas("c2", "c2");
	curTree->Draw("packetData", "", "", 10, 0);

	TCanvas* c3 = new TCanvas("c3", "c3");
	curTree->Draw("packetData", "", "", 10, 10);


	//curBranch->Print();

	inputFile->Close();
}
