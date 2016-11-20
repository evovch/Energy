#ifndef ANALYSER_H
#define ANALYSER_H

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <Rtypes.h>

class cls_AnalysedData;

class cls_Analyser
{
public:
    /*cls_Analyser();*/
    cls_Analyser(UInt_t p_lag);
    ~cls_Analyser();

    UInt_t AnalyseSSA(cls_AnalysedData* p_data);

public:
    gsl_vector* mSeriesOrig;
    gsl_vector* mSeriesReco;
    gsl_vector* mLastSigVec;

private:
    void PrintMatrix(gsl_matrix* p_M, FILE* stream = stderr);
    void PrintVector(gsl_vector* p_V, FILE* stream = stderr);

    /*
     * Full implementation - fill output matrix
     */
    void DoHankel(gsl_matrix* dest, gsl_matrix* src);

    void DoHankelVec(gsl_vector* dest, gsl_matrix* src);

    UInt_t mL;
    bool mLforced;
};

#endif // ANALYSER_H
