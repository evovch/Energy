#include "SSA_visu.h"
#include "ui_SSA_visu.h"

#include <Analyser.h>

#include <gsl/gsl_vector.h>

cls_SSA_visu::cls_SSA_visu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cls_SSA_visu)
{
    ui->setupUi(this);
}

cls_SSA_visu::~cls_SSA_visu()
{
    delete ui;
}

void cls_SSA_visu::LoadHistos(cls_Analyser* p_analyser)
{
    gsl_vector* diff = gsl_vector_alloc(p_analyser->mSeriesOrig->size);
    gsl_vector_memcpy(diff, p_analyser->mSeriesOrig);
    gsl_vector_sub(diff, p_analyser->mSeriesReco);

    ui->widget->FillFromData(p_analyser->mSeriesOrig);
    ui->widget_2->FillFromData(p_analyser->mSeriesReco);
    ui->widget_3->FillFromData(diff);

    gsl_vector_free(diff);
}
