#include "AnalysedData.h"

#include "ui_MainWindow.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "data_structures.h"

UInt_t cls_AnalysedData::mMaxNumOfAllowedMissing = 5;

cls_AnalysedData::cls_AnalysedData():
    mNumOfEntries(0),
    mNumOfEntriesUsed(0),
    mFullDataSetSize(0),
    mForcedNumOfEntries(false),
    mDataTree(nullptr),
    mNumOfMissingMaxFound(0)
{
}

cls_AnalysedData::~cls_AnalysedData()
{
}

void cls_AnalysedData::ForceNumOfEntries(UInt_t p_num)
{
    if (p_num == 0) {
        /* mNumOfEntries = 0; */
        mForcedNumOfEntries = false;
    } else {
        mNumOfEntries = p_num;
        mForcedNumOfEntries = true;
    }
}

UInt_t cls_AnalysedData::Import(const char* p_filename, UInt_t p_aggrK, Ui::cls_MainWindow* p_form)
{
    cout << "Importing " << p_filename << endl;

    // Get the file
    TFile* v_dataFile = new TFile(p_filename, "READ");
    if (v_dataFile->IsZombie()) {
        cerr << "Error opening file " << p_filename << endl;
        return 1; // FAIL
    }

    // Get the only tree of the file
    mDataTree = (TTree*)v_dataFile->Get("dataTree");
    //TODO check mDataTree

    if (p_form) p_form->leEntriesInFile->setText(QString::number(mDataTree->GetEntries()));

    // Get the number of entries in the tree
    if (!mForcedNumOfEntries) {
        mNumOfEntries = mDataTree->GetEntries();
    } else {
        // However you can not take more than there is in the file
        if (mDataTree->GetEntries() < mNumOfEntries) mNumOfEntries = mDataTree->GetEntries();
    }

    // Set the limit on the number of missing measurements
    if (p_form) mMaxNumOfAllowedMissing = p_form->leMissLimit->text().toUInt();

    this->ImportRawData(p_form);

    this->BuildAnalysisData(p_aggrK, p_form);

    v_dataFile->Close();

    /*UInt_t jj=0;
    for (auto iter: mRawData_sec) {
        cout << jj++ << "\t\t" << iter << endl;
    }*/

    return 0; // OK
}

UInt_t cls_AnalysedData::Import(QString p_qfilename, UInt_t p_aggrK, Ui::cls_MainWindow* p_form)
{
    char v_filename[255];
    strcpy(v_filename, p_qfilename.toStdString().c_str());
    return this->Import(v_filename, p_aggrK, p_form);
}

void cls_AnalysedData::ImportRawData(Ui::cls_MainWindow* p_form)
{
    UInt_t v_prevSecCounterVal=0;
    UInt_t v_fullDataSetSize=0;
    UInt_t v_counterOfMissingMeas=0;

    // Get the only branch of the tree and map the branch to the structure
    TBranch* v_recDataBranch;                // Pointer to the branch object from the tree from the ROOT file
    v_recDataBranch = mDataTree->GetBranch("recordedData");
    cout << "[ImportRawData] v_recDataBranch=" << v_recDataBranch << " mDataTree=" << mDataTree << endl;
    // One structure which is filed from the branch using Entry() method - just a temporary storage
    struct rec_packet v_dataObject;
    v_recDataBranch->SetAddress(&v_dataObject);

    // Clear arrays before filling
    mRawData_sec.clear();
    mRawData_pc.clear();

    // We know how many entries we will need, so help std::vector to allocate memory
    mRawData_sec.reserve(mNumOfEntries);
    mRawData_pc.reserve(mNumOfEntries);

    // Loop over all the entries of the branch in the tree
    for (UInt_t indexInRawData=0; indexInRawData<mNumOfEntries; indexInRawData++)
    {
        // Read entry
        v_recDataBranch->GetEntry(indexInRawData);

        // Save data into the vector of raw data
        mRawData_sec.push_back(v_dataObject.second_count);
        mRawData_pc.push_back(v_dataObject.pc_counter_1);
        /*printf ("%d:\tday %d of year %d %02d:%02d:%02d\t\t%u\t-\t%u\n", indexInRawData,
            v_dataObject.tm_yday, 1900+v_dataObject.tm_year,
            v_dataObject.tm_hour, v_dataObject.tm_min, v_dataObject.tm_sec,
            v_dataObject.second_count, v_dataObject.pc_counter_1);*/

        // First entry
        //FIXME - there is an error with yday output
        if (indexInRawData == 0) {
            printf ("day %d of year %d %02d:%02d:%02d\t\t%u\t-\t%u\n",
                v_dataObject.tm_yday, 1900+v_dataObject.tm_year,
                v_dataObject.tm_hour, v_dataObject.tm_min, v_dataObject.tm_sec,
                v_dataObject.second_count, v_dataObject.pc_counter_1);
        }

        if (indexInRawData>0) {
            UInt_t v_misMeas = v_dataObject.second_count - v_prevSecCounterVal - 1; // Number of missing measurements
            if (v_misMeas != 0) {

                // Approximate missing measurements - here only take missing measurements into account
                if (v_misMeas <= mMaxNumOfAllowedMissing) {
                    v_fullDataSetSize += v_misMeas;
                    //cout << "[OK] " << v_misMeas << " missing measurements." << endl;
                    v_counterOfMissingMeas += v_misMeas;
                    if (v_misMeas > mNumOfMissingMaxFound) mNumOfMissingMaxFound = v_misMeas;
                } else {
                    // Remove last entry because we dont need it!
                    mRawData_sec.pop_back();
                    cout << "[OK] Remove last (" << indexInRawData << "-th) entry." << endl;

                    cout << "[OK] Most probably data set is corrupted - " << v_misMeas << " (>" << mMaxNumOfAllowedMissing << ") measurements in a row are missing." << endl;
                    cout << "[OK] Taking first continious set of " << indexInRawData << " entries. "; // << endl;
                    cout << "Which means from 0 to " << indexInRawData-1 << " input entries (incl). Output data size including approximated points is " << v_fullDataSetSize << "." << endl;

                    mNumOfEntriesUsed = indexInRawData;
                    mFullDataSetSize = v_fullDataSetSize;
                    cout << "[OK] Maximum " << mNumOfMissingMaxFound << " missing measurements detected. Limit is set to " << mMaxNumOfAllowedMissing << endl;
                    cout << "[OK] Total " << v_counterOfMissingMeas << " missing measurements." << endl;
                    if (p_form) p_form->leTotalMiss->setText(QString::number(v_counterOfMissingMeas));
                    if (p_form) p_form->leMaxMiss->setText(QString::number(mNumOfMissingMaxFound));

                    // Last entry
                    printf ("day %d of year %d %02d:%02d:%02d\t\t%u\t-\t%u\n",
                        v_dataObject.tm_yday, 1900+v_dataObject.tm_year,
                        v_dataObject.tm_hour, v_dataObject.tm_min, v_dataObject.tm_sec,
                        v_dataObject.second_count, v_dataObject.pc_counter_1);

                    return;
                }
            }
        }
        v_prevSecCounterVal = v_dataObject.second_count;
        v_fullDataSetSize+=1;
    }

    mNumOfEntriesUsed = mNumOfEntries;
    mFullDataSetSize = v_fullDataSetSize;
    cout << "[OK] Maximum " << mNumOfMissingMaxFound << " missing measurements detected. Limit is set to " << mMaxNumOfAllowedMissing << endl;
    cout << "[OK] Total " << v_counterOfMissingMeas << " missing measurements." << endl;
    if (p_form) p_form->leTotalMiss->setText(QString::number(v_counterOfMissingMeas));
    if (p_form) p_form->leMaxMiss->setText(QString::number(mNumOfMissingMaxFound));

    // Last entry
    printf ("day %d of year %d %02d:%02d:%02d\t\t%u\t-\t%u\n",
        v_dataObject.tm_yday, 1900+v_dataObject.tm_year,
        v_dataObject.tm_hour, v_dataObject.tm_min, v_dataObject.tm_sec,
        v_dataObject.second_count, v_dataObject.pc_counter_1);

    return;
}

void cls_AnalysedData::BuildAnalysisData(UInt_t p_aggrK, Ui::cls_MainWindow* p_form)
{
    // Clear arrays before filling
    mFullData.clear();
    mSparceData.clear();

    // We know how many entries we will need, so help std::vector to allocate memory
    mFullData.reserve(mFullDataSetSize);
    mSparceData.reserve(mFullDataSetSize/p_aggrK);

    UInt_t prevSecCounterVal=0;
    UInt_t prevPcCounterVal=0;
    UInt_t v_secDiff;
    UInt_t v_pcDiff;

    UInt_t v_skipped=0;

    for (UInt_t indexInRawData=0; indexInRawData<mNumOfEntriesUsed; indexInRawData++)
    {
        if (indexInRawData==0) { // First entry
            cout << "First measurement used as reference." << endl;
        } else {
            v_secDiff = mRawData_sec[indexInRawData] - prevSecCounterVal;
            v_pcDiff = mRawData_pc[indexInRawData] - prevPcCounterVal;

            if (mRawData_pc[indexInRawData] < prevPcCounterVal) {
                cout << mRawData_pc[indexInRawData] << " - " << prevPcCounterVal << "\t";
                cout << indexInRawData << ": " << v_secDiff << "\t\t" << v_pcDiff << endl;
                cout << "Next value is less than the previous. Skipping measurement." << endl;
                v_skipped++;
                continue;
            }

            if (v_pcDiff > 100000)
            {
                cout << mRawData_pc[indexInRawData] << " - " << prevPcCounterVal << "\t";
                cout << indexInRawData << ": " << v_secDiff << "\t\t" << v_pcDiff << endl;
                cout << "!!!!!!!!!!!!!" << endl;
                continue;
            }

            UInt_t v_misMeas = v_secDiff - 1;
            if (v_misMeas != 0) { // This means that a measurement or several were skipped

                if (v_misMeas > mMaxNumOfAllowedMissing) {
                    cout << "[!!] Something went completely wrong! Debug it!" << endl;
                    cerr << "[!!] Something went completely wrong! Debug it!" << endl;
                    return;
                }

                //cout << "[OK] Missing " << v_misMeas << " measurements. pcDiff=" << v_pcDiff << endl;

                // Approximate missing measurement with linear function
                for (UInt_t k=0; k<v_secDiff; k++) {
                    mFullData.push_back(((Double_t)v_pcDiff)/(Double_t)v_secDiff);
                }

            } else {
                //cout << "pcDiff=" << v_pcDiff << endl;
                mFullData.push_back((Double_t)v_pcDiff);
            }
        }
        prevSecCounterVal = mRawData_sec[indexInRawData];
        prevPcCounterVal = mRawData_pc[indexInRawData];
    }

    std::vector<Double_t>::iterator v_fullDataI;
    UInt_t v_counter=0;
    UInt_t v_curSum=0;
    for (v_fullDataI = mFullData.begin(); v_fullDataI != mFullData.end(); ++v_fullDataI) {

        v_curSum += *v_fullDataI;

        if ((v_counter+1) % p_aggrK == 0) {
            mSparceData.push_back((Double_t)v_curSum/(Double_t)p_aggrK);
            v_curSum = 0;
        }

        v_counter++;

    }

    if (p_form) p_form->leDataSize->setText(QString::number(mFullData.size()));
    if (p_form) p_form->leAggrDataSize->setText(QString::number(mSparceData.size()));
    if (p_form) p_form->leUsed->setText(QString::number(mNumOfEntriesUsed));
    if (p_form) p_form->leSkipped->setText(QString::number(v_skipped));

    cout << "mNumOfEntries=" << mNumOfEntries << endl;
    cout << "mNumOfEntriesUsed=" << mNumOfEntriesUsed << endl;
    cout << "mFullDataSetSize=" << mFullDataSetSize << endl;
    cout << "mRawData_sec.size()=" << mRawData_sec.size() << endl;
    cout << "mRawData_pc.size()=" << mRawData_pc.size() << endl;
    cout << "mFullData.size()=" << mFullData.size() << " (minus 1 from 'mFullDataSetSize' is OK if you plot 'diff')" << endl;
    cout << "mSparceData.size()=" << mSparceData.size() << endl;
    cout << endl;

    return;
}
