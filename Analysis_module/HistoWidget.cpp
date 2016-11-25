#include "HistoWidget.h"
#include "ui_HistoWidget.h"

#include <TStyle.h>
#include <TPad.h>
#include <TH1D.h>
#include <TRandom3.h>

#include <iostream>
using std::cout;
using std::endl;

#include <QFileDialog>

//#include <TPaveStats.h>

UInt_t cls_HistoWidget::mHistoID = 0;

cls_HistoWidget::cls_HistoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cls_HistoWidget),
    mHisto(new TH1D())
{
    ui->setupUi(this);

    // Here we create a default histo with some parameters
    TString v_histoName;
    TString v_histoTitle;
    v_histoName.Form("mHisto_%d", mHistoID);
    //v_histoTitle.Form("mHisto_%d", mHistoID);
    mHistoID++;
    std::unique_ptr<TH1D> v_tmpHisto(new TH1D(v_histoName.Data(), v_histoTitle.Data(), 100, 0., 100.));
    mHisto = std::move(v_tmpHisto);
    ui->canvasWidget->cd();
    gPad->SetGrid(1,1);
    mHisto->Draw();
    //              ksiourmen
    gStyle->SetOptStat(111110);
    //TPaveStats* ps1 = (TPaveStats*)(mHisto->GetListOfFunctions()->FindObject("stats"));
    //ps1->SetDrawOption();
}

cls_HistoWidget::~cls_HistoWidget()
{
    delete ui;
}

void cls_HistoWidget::FillTest(void)
{
    Int_t v_numOfBins = mHisto->GetNbinsX();
    TRandom3 v_randomGenerator;
    v_randomGenerator.SetSeed();
    mHisto->Reset();
    for (Int_t i=0; i<v_numOfBins; i++) {
        mHisto->SetBinContent(i, v_randomGenerator.Rndm());
    }

    ui->canvasWidget->cd();
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::FillFromData(std::vector<Double_t> p_data)
{
    // Maximum element in the data
    Double_t v_maxElem = *std::max_element(p_data.begin(), p_data.end()); // 2000.
    // Minimum element in the data
    Double_t v_minElem = *std::min_element(p_data.begin(), p_data.end()); // 0.

    Double_t v_diff = v_maxElem - v_minElem;

    TString v_histoName = mHisto->GetName();
    TString v_histoTitle = mHisto->GetTitle();
    std::unique_ptr<TH1D> v_tmpHisto(new TH1D(v_histoName.Data(), v_histoTitle.Data(), (UInt_t)(v_diff+1.), v_minElem, v_maxElem + 1.));
    mHisto = std::move(v_tmpHisto);

    mHisto->Reset();
    for (UInt_t i=0; i<p_data.size(); i++) {
        mHisto->Fill(p_data[i]);
    }

    ui->canvasWidget->cd();
    mHisto->Draw();

    mHisto->SetLineWidth(2);

    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();

    //              ksiourmen
    gStyle->SetOptStat(111110);
}

void cls_HistoWidget::FillFromData(std::vector<struct rec_packet> p_data)
{
    if (p_data.size() == 0) {
        cout << "Empty data set. Aborting." << endl;
        return;
    }

    std::vector<Double_t> v_tmpVector;

    uint32_t v_prev = p_data[0].pc_counter_1;
    uint32_t v_prevSec = p_data[0].second_count;
    for (UInt_t i=1; i<p_data.size(); i++) {

        if (p_data[i].pc_counter_1 < v_prev) {
            cout << "Next pc_counter_1 value is less than the previous! " << p_data[i].pc_counter_1 << "<" << v_prev << endl;
        }
        if (p_data[i].second_count < v_prevSec) {
            cout << "Next second_count value is less than the previous! " << p_data[i].second_count << "<" << v_prevSec << endl;
        }

        uint32_t v_diff = p_data[i].pc_counter_1 - v_prev;
        uint32_t v_sec_diff = p_data[i].second_count - v_prevSec;

        Double_t v_ratio;

        if (v_sec_diff != 1) {
            v_ratio = (Double_t)v_diff / (Double_t)v_sec_diff;
            cout << "v_sec_diff = " << v_sec_diff << "\tv_diff = " << v_diff << endl;
            for (UInt_t j=0; j<v_sec_diff; j++) {
                v_tmpVector.push_back(v_ratio);
            }

        } else {
            v_ratio = (Double_t)v_diff;
            v_tmpVector.push_back(v_ratio);
        }

        v_prev = p_data[i].pc_counter_1;
        v_prevSec = p_data[i].second_count;
    }

    this->FillFromData(v_tmpVector);
}

void cls_HistoWidget::SetLogY(Int_t p_val)
{
    ui->canvasWidget->cd();
    gPad->SetLogy(p_val);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::ToggleLogY(void)
{
    ui->canvasWidget->cd();
    gPad->GetLogy()==0 ? gPad->SetLogy(1) : gPad->SetLogy(0);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::UnZoom(void)
{
    ui->canvasWidget->cd();
    mHisto->GetXaxis()->UnZoom();
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::RebinBy2(void)
{
    ui->canvasWidget->cd();
    mHisto->Rebin(2);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::ExportEPS(void)
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

void cls_HistoWidget::PlusThickness(void)
{
    mHisto->SetLineWidth(mHisto->GetLineWidth()+1);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::MinusThickness(void)
{
    if (mHisto->GetLineWidth() > 1) {
        mHisto->SetLineWidth(mHisto->GetLineWidth()-1);
        ui->canvasWidget->Modified();
        ui->canvasWidget->Update();
    }
}

void cls_HistoWidget::Rename(void)
{
    mHisto->SetName("Power distribution");
    mHisto->SetTitle("Power distribution");

    TString v_YaxisTitle;
    v_YaxisTitle.Form("1/%d units", (UInt_t)(mHisto->GetBinWidth(1)));

    mHisto->GetXaxis()->SetTitle("units");
    mHisto->GetYaxis()->SetTitle(v_YaxisTitle);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}

void cls_HistoWidget::MoveYtitleLeft(void)
{
    mHisto->GetYaxis()->SetTitleOffset(mHisto->GetYaxis()->GetTitleOffset()+0.1);
    ui->canvasWidget->Modified();
    ui->canvasWidget->Update();
}
