#ifndef ANALYSEDDATA_H
#define ANALYSEDDATA_H

#include <QString>

#include <Rtypes.h>
#include <vector>

class TTree;

namespace Ui {
class cls_MainWindow;
}

class cls_AnalysedData
{
public:
    cls_AnalysedData();
    ~cls_AnalysedData();

    /*
     * return value 1 - error
     * 0 - ok
     */
    UInt_t Import(const char* filename, UInt_t p_aggrK = 1, Ui::cls_MainWindow* p_form = nullptr);

    /*
     * Wrapper
     */
    UInt_t Import(QString p_qfilename, UInt_t p_aggrK = 1, Ui::cls_MainWindow* p_form = nullptr);

    /*
     *
     */
    void ImportRawData(Ui::cls_MainWindow* p_form = nullptr);

    /*
     *
     */
    void BuildAnalysisData(UInt_t p_aggrK = 1, Ui::cls_MainWindow* p_form = nullptr);

    /*
     * Used to overwrite the number of entries to be imported from the ROOT file.
     * If set to 0 - import all.
     */
    void ForceNumOfEntries(UInt_t p_num);

    std::vector<Double_t> GetFullData(void) const { return mFullData; }
    std::vector<Double_t> GetSparceData(void) const { return mSparceData; }

    /*
     * Specify what data you need here
     */
    std::vector<Double_t> GetData(void) const { return this->GetSparceData(); }
    UInt_t GetDataSize(void) const { return this->GetSparceData().size(); }
    Double_t GetDataMember(UInt_t i) const { return mSparceData[i]; }

private:
    cls_AnalysedData(cls_AnalysedData const &) = delete;
    cls_AnalysedData &operator=(cls_AnalysedData const &) = delete;

private:
    UInt_t mNumOfEntries;                   // Number of input ROOT data tree entries to be looked into
    UInt_t mNumOfEntriesUsed;               // Actual number of input ROOT data tree entries to be used
    UInt_t mFullDataSetSize;                // Number of measurements including approximated to be stored and used by analysis

    Bool_t mForcedNumOfEntries;             // True if the number of entries to be read from the file was set externally

    TTree* mDataTree;                       // Pointer to the data tree object from the ROOT file

    std::vector<Double_t> mSparceData;      // With aggregation

    std::vector<Double_t> mFullData;        // [mFullDataSetSize-1] or [mFullDataSetSize]. Minus 1 if you plan to skip first measurement (in case of ratio)

    std::vector<UInt_t> mRawData_sec;       // [mNumOfEntries] second_count
    std::vector<UInt_t> mRawData_pc;        // [mNumOfEntries] pc_counter_1

    UInt_t mNumOfMissingMaxFound;           // Maximum number of missing measurements in a row detected in the input file
    static UInt_t mMaxNumOfAllowedMissing;  // Maximum number of allowed missing measurement which will be approximated

};

#endif // ANALYSEDDATA_H
