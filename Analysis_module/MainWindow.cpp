#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AnalysedData.h"
#include "Analyser.h"
#include "SSA_visu.h"
#include "GraphWidget.h"

#include <QString>

cls_MainWindow::cls_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cls_MainWindow),
    mDataObject(new cls_AnalysedData())
{
    ui->setupUi(this);
}

cls_MainWindow::~cls_MainWindow()
{
    delete ui;
}

void cls_MainWindow::LoadFile(void)
{
    unsigned int v_num = ui->leEntriesLimit->text().toUInt();
    mDataObject->ForceNumOfEntries(v_num);
    mDataObject->Import(ui->leFilename->text(), ui->leAggrK->text().toUInt(), ui);
    ui->theHistoWidget1->FillFromData(mDataObject->GetData());
    ui->theGraphWidget1->FillFromData(mDataObject->GetData());
}

void cls_MainWindow::Reload(void)
{
    ui->theHistoWidget1->FillFromData(mDataObject->GetData());
    ui->theGraphWidget1->FillFromData(mDataObject->GetData());
}

void cls_MainWindow::RunSSA(void)
{
    cls_Analyser* v_SSAanalyser = new cls_Analyser(ui->leSSAL->text().toUInt());

    if (v_SSAanalyser->AnalyseSSA(mDataObject.get()) == 0)
    {
        cls_SSA_visu* SSAvisu = new cls_SSA_visu();
        SSAvisu->LoadHistos(v_SSAanalyser);
        SSAvisu->show();

        cls_GraphWidget* sigGraph = new cls_GraphWidget("Components' weights");
        sigGraph->SetDrawOptions("LA*");
        sigGraph->FillFromData(v_SSAanalyser->mLastSigVec);
        sigGraph->show();
    }

    delete v_SSAanalyser;
}
