#include "Analyser.h"

#include <stdio.h>

/*
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
*/

#include <vector>

#include "AnalysedData.h"
#include "GraphWidget.h"

//#include <gsl/gsl_vector.h>
//#include <gsl/gsl_matrix.h>

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
/*
cls_Analyser::cls_Analyser() :
    mSeriesOrig(nullptr),
    mSeriesReco(nullptr),
    mL(20),
    mLforced(false)
{
}
*/
cls_Analyser::cls_Analyser(UInt_t p_L) :
    mSeriesOrig(nullptr),
    mSeriesReco(nullptr),
    mLastSigVec(nullptr),
    mL(p_L),
    mLforced(true)
{
}

cls_Analyser::~cls_Analyser()
{
    gsl_vector_free(mSeriesOrig);
    gsl_vector_free(mSeriesReco);
    gsl_vector_free(mLastSigVec);
}

// m - number of rows       - i
// n - number of columns    - j
void cls_Analyser::PrintMatrix(gsl_matrix* p_M, FILE* stream)
{
    ///*
    for (UInt_t i=0; i<p_M->size1; i++) {
        for (UInt_t j=0; j<p_M->size2; j++) {
            fprintf (stream, "% 8.3f\t", gsl_matrix_get(p_M, i, j));
        }
        fprintf (stream, "\n");
    }
    fprintf (stream, "\n");
    //*/
}

void cls_Analyser::PrintVector(gsl_vector* p_V, FILE* stream)
{
    ///*
    for (UInt_t i=0; i<p_V->size; i++) {
        fprintf (stream, "% 8.3f\n", gsl_vector_get(p_V, i));
    }
    fprintf (stream, "\n");
    //*/
}

void cls_Analyser::DoHankel(gsl_matrix* dest, gsl_matrix* src)
{
    UInt_t K = src->size1;
    UInt_t L = src->size2;

    Double_t sum = 0.;

    for (UInt_t i=0; i<L-1; i++) {
        sum = 0.;
        for (UInt_t j=0; j<=i; j++) {
            //gsl_matrix_set(dest, i-j, j, 1.);         // just for testing
            sum += gsl_matrix_get(src, i-j, j);
        }
        for (UInt_t j=0; j<=i; j++) {
            gsl_matrix_set(dest, i-j, j, sum/(Double_t)(i+1));
        }
    }

    for (UInt_t i=L-1; i<K; i++) {
        sum = 0.;
        for (UInt_t j=0; j<L; j++) {
            //gsl_matrix_set(dest, i-j, j, 2.);         // just for testing
            sum += gsl_matrix_get(src, i-j, j);
        }
        for (UInt_t j=0; j<L; j++) {
            gsl_matrix_set(dest, i-j, j, sum/(Double_t)L);
        }
    }

    for (UInt_t i=0; i<L-1; i++) {
        sum = 0.;
        for (UInt_t j=0; j<=i; j++) {
            //gsl_matrix_set(dest, K-1-(i-j), L-1-j, 3.);         // just for testing
            sum += gsl_matrix_get(src, K-1-(i-j), L-1-j);
        }
        for (UInt_t j=0; j<=i; j++) {
            gsl_matrix_set(dest, K-1-(i-j), L-1-j, sum/(Double_t)(i+1));
        }
    }

    //PrintMatrix(dest);
}

void cls_Analyser::DoHankelVec(gsl_vector* dest, gsl_matrix* src)
{
    UInt_t K = src->size1;
    UInt_t L = src->size2;

    Double_t sum = 0.;

    for (UInt_t i=0; i<L-1; i++) {
        sum = 0.;
        for (UInt_t j=0; j<=i; j++) {
            sum += gsl_matrix_get(src, i-j, j);
        }
        gsl_vector_set(dest, i, sum/(Double_t)(i+1));
    }

    for (UInt_t i=L-1; i<K; i++) {
        sum = 0.;
        for (UInt_t j=0; j<L; j++) {
            sum += gsl_matrix_get(src, i-j, j);
        }
        gsl_vector_set(dest, i, sum/(Double_t)L);
    }

    for (UInt_t i=0; i<L-1; i++) {
        sum = 0.;
        for (UInt_t j=0; j<=i; j++) {
            sum += gsl_matrix_get(src, K-1-(i-j), L-1-j);
        }
        gsl_vector_set(dest, K+L-2-i, sum/(Double_t)(i+1));
    }

    //PrintVector(dest);
}

UInt_t cls_Analyser::AnalyseSSA(cls_AnalysedData* p_data)
{
    // N - series length
    // L - window length
    // K = N - L + 1

    UInt_t N = p_data->GetDataSize();

    if (N == 0) {
        fprintf (stderr, "No input data for SSA analysis.\n");
        return 1;
    }

    UInt_t L = mL;
    UInt_t K = N - L + 1;

    fprintf (stderr, "Performing SSA. N=%d, L=%d, K=%d\n", N, L, K);

    // ===============================================================================================================================
    // Get the original series into the GSL vector
    // Allocate mSeriesOrig
    mSeriesOrig = gsl_vector_alloc(K+L-1);

    // Fill mSeriesOrig
    for (UInt_t i=0; i<K+L-1; i++) {
        gsl_vector_set(mSeriesOrig, i, p_data->GetDataMember(i));
    }

    // Free of mSeriesOrig - in destructor

    // ===============================================================================================================================
    // Allocate X
    gsl_matrix* X = gsl_matrix_alloc (L, K);

    // Fill X
    for (UInt_t i=0; i<L; i++) {
        for (UInt_t j=0; j<K; j++) {
            gsl_matrix_set(X, i, j, gsl_vector_get(mSeriesOrig, i+j));
        }
    }

    // Print X
    //fprintf (stderr, "----------------------------------------- X -----------------------------------------\n");
    //PrintMatrix(X);

    // Free in the end of this method

    // ===============================================================================================================================
    // Allocate Xt
    gsl_matrix* Xt = gsl_matrix_alloc(K, L);
    // Fill Xt
    gsl_matrix_transpose_memcpy (Xt, X);

    // Print Xt
    //fprintf (stderr, "----------------------------------------- Xt ----------------------------------------\n");
    //PrintMatrix(Xt);

    // Free in the end of this method

    // ===============================================================================================================================
/*
    // Allocate covM = X * Xt
    gsl_matrix* covM = gsl_matrix_alloc (L, L);
    // Fill covM
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1., X, X, 1., covM);

    // Print covM
    //fprintf (stderr, "----------------------------------------- X * Xt ------------------------------------\n");
    //PrintMatrix(covM);

    // Free in the end of this method
*/
    // ===============================================================================================================================
    // SVD ---------------------------------------------------------------------------------------------------------------------------

    // Allocate U, V, vecS, work
    gsl_matrix* U = gsl_matrix_alloc(K, L);
    gsl_matrix* V = gsl_matrix_alloc(L, L);
    gsl_vector* vecS = gsl_vector_alloc(L);
    gsl_vector* work = gsl_vector_alloc(L);

    // Fill U
    gsl_matrix_memcpy(U, Xt);

    // Run SVD which fills V and S and chages data inside U from actually Xt to U and uses 'work' vector
    gsl_linalg_SV_decomp(U, V, vecS, work);

    mLastSigVec = gsl_vector_alloc(L);
    gsl_vector_memcpy(mLastSigVec, vecS);

/*
    // Print U
    fprintf (stderr, "----------------------------------------- U -----------------------------------------\n");
    PrintMatrix(U);

    // Print V
    fprintf (stderr, "----------------------------------------- V -----------------------------------------\n");
    PrintMatrix(V);

    // Print vecS
    fprintf (stderr, "----------------------------------------- vecS --------------------------------------\n");
    PrintVector(vecS);
*/
/*
    // -------------------------------------------------------------------------------------------------------------------------------
    // Check result 1
    // Ut * U = E
    // V * Vt = E

    // Allocate UtU, VVt
    gsl_matrix* UtU = gsl_matrix_alloc(L, L);
    gsl_matrix* VVt = gsl_matrix_alloc(L, L);
    // Fill UtU, VVt
    gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., U, U, 1., UtU);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1., V, V, 1., VVt);

    // Print Ut * U = E
    //fprintf (stderr, "----------------------------------------- Ut * U = E --------------------------------\n");
    //PrintMatrix(UtU);

    // Print V * Vt = E
    //fprintf (stderr, "----------------------------------------- V * Vt = E --------------------------------\n");
    //PrintMatrix(VVt);

    // Free UtU, VVt
    gsl_matrix_free(UtU);
    gsl_matrix_free(VVt);

    // -------------------------------------------------------------------------------------------------------------------------------
*/

    // -------------------------------------------------------------------------------------------------------------------------------
    // Check result 2
    // X = X1 + X2 + ... + Xd

    // Allocate
    gsl_vector* vecUi = gsl_vector_alloc(K);
    gsl_vector* vecVi = gsl_vector_alloc(L);

    gsl_matrix** Xcomp;
    Xcomp = new gsl_matrix*[L];
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        Xcomp[compIndex] = gsl_matrix_alloc(K, L);
    }

    // Compute components
    for (UInt_t compIndex=0; compIndex<L; compIndex++)
    {
        Double_t lambda = gsl_vector_get(vecS, compIndex);

        gsl_matrix_get_col(vecUi, U, compIndex);
        gsl_matrix_get_col(vecVi, V, compIndex);

        for (UInt_t i=0; i<K; i++) {
            for (UInt_t j=0; j<L; j++) {
                Double_t val = lambda * gsl_vector_get(vecUi, i) * gsl_vector_get(vecVi, j);
                gsl_matrix_set(Xcomp[compIndex], i, j, val);
            }
        }
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1
    // Perform Hankelization

    // Allocate
    /*
    gsl_matrix** HXcomp;
    HXcomp = new gsl_matrix*[L];
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        HXcomp[compIndex] = gsl_matrix_alloc(K, L);
    }
    */

    gsl_vector** HXcompVec;
    HXcompVec = new gsl_vector*[L];
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        HXcompVec[compIndex] = gsl_vector_alloc(K+L-1);
    }

    cls_GraphWidget** compWidgets = new cls_GraphWidget*[L];

    // Per-se Hankelization - diagonal averaging
    // Fill all 'HXcomp'
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        //DoHankel(HXcomp[compIndex], Xcomp[compIndex]);        // Just for testing
        DoHankelVec(HXcompVec[compIndex], Xcomp[compIndex]);
    }

    // Just fraw 4 first components
    char title[32];
    for (UInt_t compIndex=0; compIndex<4; compIndex++) {
        sprintf (title, "Component %d", compIndex+1);
        compWidgets[compIndex] = new cls_GraphWidget(title);
        compWidgets[compIndex]->SetDrawOptions("LA");
        compWidgets[compIndex]->FillFromData(HXcompVec[compIndex]);
        compWidgets[compIndex]->show();
    }

    // Sum series-components
    // Allocate mSeriesReco
    mSeriesReco = gsl_vector_alloc(K+L-1);

    // Fill mSeriesReco
    gsl_vector_set_zero(mSeriesReco);
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        gsl_vector_add(mSeriesReco, HXcompVec[compIndex]);
    }

    // Free of mSeriesReco - in destructor

    // Free
    /*
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        gsl_matrix_free(HXcomp[compIndex]);
    }
    delete [] HXcomp;
    */
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        gsl_vector_free(HXcompVec[compIndex]);
    }
    delete [] HXcompVec;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++1

    // Sum components to get full Xreco = X1 + X2 + ... + Xd  (d = L)
/*
    gsl_matrix* Xreco = gsl_matrix_alloc(K, L);
    gsl_matrix_set_zero(Xreco);
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        gsl_matrix_add(Xreco, Xcomp[compIndex]);
        //fprintf (stderr, "----------------------------------------- Xcomp[%d] ----------------------------------\n", compIndex);
        //PrintMatrix(Xcomp[compIndex]);
    }
*/
/*
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++2
    // Compare reconstructed and original Xt
    // deltaX = Xt - Xreco
    gsl_matrix* deltaX = gsl_matrix_alloc(K, L);
    gsl_matrix_memcpy(deltaX, Xt);
    gsl_matrix_sub(deltaX, Xreco);

    //fprintf (stderr, "----------------------------------------- deltaX ------------------------------------\n");
    //PrintMatrix(deltaX);

    gsl_matrix_free(deltaX);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++2
*/

    // Free
    gsl_vector_free(vecUi);
    gsl_vector_free(vecVi);
    for (UInt_t compIndex=0; compIndex<L; compIndex++) {
        gsl_matrix_free(Xcomp[compIndex]);
    }
    delete [] Xcomp;
    //gsl_matrix_free(Xreco);

    // -------------------------------------------------------------------------------------------------------------------------------

    // Free U, V, vecS, work
    gsl_matrix_free(U);
    gsl_matrix_free(V);
    gsl_vector_free(vecS);
    gsl_vector_free(work);

    // Free X, Xt, covM
    gsl_matrix_free(X);
    gsl_matrix_free(Xt);
    //gsl_matrix_free(covM);

    return 0;
}
