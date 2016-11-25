#include "GraphWidget.h"
#include "ui_GraphWidget.h"

#include <QByteArray>
#include <QFileDialog>

#include <TStyle.h>
#include <TPad.h>
#include <TGraph.h>
#include <TRandom3.h>
#include <TAxis.h>

#include <iostream>
using std::cout;
using std::endl;

UInt_t cls_GraphWidget::mGraphID = 0;

cls_GraphWidget::cls_GraphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cls_GraphWidget),
    mGraph(new TGraph(100)),
    mDrawOptions("")
{
    ui->setupUi(this);

    mGraphID++;

    char opts[32];
    QByteArray ba = mDrawOptions.toLocal8Bit();
    strcpy(opts, ba.constData());

    ui->canvasWidget->cd();
    gStyle->SetOptStat(1111111);
    gPad->SetGrid(1,1);
    mGraph->Draw(opts);
}

cls_GraphWidget::cls_GraphWidget(char* p_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cls_GraphWidget),
    mGraph(new TGraph(100)),
    mDrawOptions("")
{
    ui->setupUi(this);

    mGraphID++;

    char opts[32];
    QByteArray ba = mDrawOptions.toLocal8Bit();
    strcpy(opts, ba.constData());

    mGraph->SetTitle(p_name);

    ui->canvasWidget->cd();
    gStyle->SetOptStat(1111111);
    gPad->SetGrid(1,1);
    mGraph->Draw(opts);
}

cls_GraphWidget::~cls_GraphWidget()
{
    delete ui;
}

void cls_GraphWidget::FillTest(void)
{
    Int_t v_numOfPoints = mGraph->GetN();
    TRandom3 v_randomGenerator;
    v_randomGenerator.SetSeed();

    for (Int_t i=0; i<v_numOfPoints; i++) {
        mGraph->SetPoint(i, i, v_randomGenerator.Rndm());
    }

    ui->canvasWidget->cd();
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_GraphWidget::FillFromData(std::vector<Double_t> p_data)
{
    mGraph->Set(p_data.size());
    for (UInt_t i=0; i<p_data.size(); i++) {
        //cout << i << "\t\t" << p_data[i] << endl;
        mGraph->SetPoint(i, (Double_t)(i), p_data[i]);
    }

    char opts[32];
    QByteArray ba = mDrawOptions.toLocal8Bit();
    strcpy(opts, ba.constData());

    ui->canvasWidget->cd();
    mGraph->Draw(opts);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_GraphWidget::FillFromData(gsl_vector* p_data)
{
    mGraph->Set(p_data->size);
    for (UInt_t i=0; i<p_data->size; i++) {
        mGraph->SetPoint(i, (Double_t)(i), gsl_vector_get(p_data, i));
    }

    char opts[32];
    QByteArray ba = mDrawOptions.toLocal8Bit();
    strcpy(opts, ba.constData());

    ui->canvasWidget->cd();
    mGraph->Draw(opts);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

// No approximation of missing measurements here
void cls_GraphWidget::FillFromData(std::vector<struct rec_packet> p_data, enu_FillFromPacketType p_fillType)
{
    std::vector<Double_t> v_tmpVector;

    Double_t v_prev;
    switch (p_fillType) {
        case etnSecStraight:
            for (UInt_t i=0; i<p_data.size(); i++) {
                v_tmpVector.push_back(p_data[i].second_count);
            }
            break;
        case etnPcStraight:
            for (UInt_t i=0; i<p_data.size(); i++) {
                v_tmpVector.push_back(p_data[i].pc_counter_1);
            }
            break;
        case etnSecDiff:
            v_prev = p_data[0].second_count;
            for (UInt_t i=1; i<p_data.size(); i++) {
                v_tmpVector.push_back(p_data[i].second_count - v_prev);
                v_prev = p_data[i].second_count;
            }
            break;
        case etnPcDiff:
            v_prev = p_data[0].pc_counter_1;
            for (UInt_t i=1; i<p_data.size(); i++) {
                v_tmpVector.push_back(p_data[i].pc_counter_1 - v_prev);
                v_prev = p_data[i].pc_counter_1;
            }
            break;
    }

    this->FillFromData(v_tmpVector);
}

void cls_GraphWidget::SetLogY(Int_t p_val)
{
    ui->canvasWidget->cd();
    gPad->SetLogy(p_val);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_GraphWidget::ToggleLogY(void)
{
    ui->canvasWidget->cd();
    gPad->GetLogy()==0 ? gPad->SetLogy(1) : gPad->SetLogy(0);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_GraphWidget::UnZoom(void)
{
    ui->canvasWidget->cd();
    mGraph->GetXaxis()->UnZoom();
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_GraphWidget::ExportEPS(void)
{
    QString v_qfilename = QFileDialog::getSaveFileName(this,
                            tr("Save File"),
                            "/home/evovch/Documents/DEVELOPMENT/Ivanov/histo.eps",
                            tr("Encapsulated PostScript files (*.eps)"));
/*
    QFileDialog v_dial;
    v_dial.setFileMode(QFileDialog::AnyFile);
    v_dial.setNameFilter(tr("Encapsulated PostScript files (*.eps)"));
    v_dial.setDirectory("/home/evovch/Documents/DEVELOPMENT/Ivanov/");

    QStringList v_fileNames;
    if (v_dial.exec())
        v_fileNames = v_dial.selectedFiles();

    QString v_qfilename = v_fileNames.at(0);
*/
    if (!v_qfilename.endsWith(".eps")) {
        v_qfilename.append(".eps");
    }

    TString v_root_filename(v_qfilename.toStdString());

    ui->canvasWidget->Print(v_root_filename);
}
