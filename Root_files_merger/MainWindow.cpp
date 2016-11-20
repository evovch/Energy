#include "MainWindow.h"
#include "ui_cls_mainwindow.h"

#include <string>
#include <fstream>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;


#include <QFileDialog>
#include <QStringList>
#include <QDebug>

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>

#include "GraphWidget.h"
#include "HistoWidget.h"

std::vector<std::string>
tokenize(const std::string & str, const std::string & delimiters)
{
  std::vector<std::string> tokens;

  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    // Found a token, add it to the vector.
    tokens.push_back(str.substr(lastPos, pos - lastPos));

    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);

    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }

  return tokens;
}

cls_MainWindow::cls_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cls_MainWindow)
{
    ui->setupUi(this);
}

cls_MainWindow::~cls_MainWindow()
{
    delete ui;
}

void cls_MainWindow::ChooseFile(void)
{
    QFileDialog v_dial;
    v_dial.setFileMode(QFileDialog::ExistingFiles);
    v_dial.setNameFilter(tr("Root files (*.root)"));
    v_dial.setDirectory("/home/evovch/Documents/DEVELOPMENT/Ivanov/data");

    QStringList v_fileNames;
    if (v_dial.exec())
        v_fileNames = v_dial.selectedFiles();

    //TODO - do not add existing files

    ui->listWidget->addItems(v_fileNames);
}

unsigned int cls_MainWindow::Merge(void)
{
    //TODO maybe first clear mImportedData
    mImportedData.clear();

    for (int i=0; i<ui->listWidget->count(); i++) {

        QListWidgetItem* v_item = ui->listWidget->item(i);
        QString v_curLine = (QString)v_item->text();
        unsigned int res = this->Append(v_curLine);

        if (res) {
            qDebug() << "Failed appending file " /* << v_curLine.toAscii() */ << endl;
            return 1;
        }
    }

    if (this->Export(ui->lineEdit->text())) {
        qDebug() << "Failed exporting file " /* << ui->lineEdit->text() */ << endl;
        return 1;
    }
/*
    cls_GraphWidget* v_grWin1 = new cls_GraphWidget();
    v_grWin1->FillFromData(mImportedData, etnSecStraight);
    v_grWin1->show();

    cls_GraphWidget* v_grWin2 = new cls_GraphWidget();
    v_grWin2->FillFromData(mImportedData, etnPcStraight);
    v_grWin2->show();

    cls_GraphWidget* v_grWin3 = new cls_GraphWidget();
    v_grWin3->FillFromData(mImportedData, etnSecDiff);
    v_grWin3->show();

    cls_GraphWidget* v_grWin4 = new cls_GraphWidget();
    v_grWin4->FillFromData(mImportedData, etnPcDiff);
    v_grWin4->show();
*/
    cls_HistoWidget* v_histoWin1 = new cls_HistoWidget();
    v_histoWin1->FillFromData(mImportedData);
    v_histoWin1->show();

    return 0;
}

// 1 - fail
// 0 - ok
unsigned int cls_MainWindow::Append(QString p_filename)
{
    char v_filename[255];
    strcpy(v_filename, p_filename.toStdString().c_str());

    qDebug() << "Appending " << v_filename;

    // Get the file
    TFile* v_dataFile = new TFile(v_filename, "READ");
    if (v_dataFile->IsZombie()) {
        qDebug() << "Error opening file " << v_filename << endl;
        return 1; // FAIL
    }

    // Get the only tree of the file
    TTree* v_DataTree = (TTree*)v_dataFile->Get("dataTree");
    //TODO check mDataTree

    unsigned int v_NumOfEntries = v_DataTree->GetEntries();

    qDebug() << v_NumOfEntries << " entries in the file." << endl;

    // Get the only branch of the tree and map the branch to the structure
    TBranch* v_recDataBranch;                // Pointer to the branch object from the tree from the ROOT file
    v_recDataBranch = v_DataTree->GetBranch("recordedData");
    // One structure which is filed from the branch using Entry() method - just a temporary storage
    struct rec_packet v_dataObject;
    v_recDataBranch->SetAddress(&v_dataObject);

    // Compute filtering parameters no matter if the checkbox is on or off
    unsigned int v_filterFromH = ui->leFromH->text().toUInt();
    unsigned int v_filterFromM = ui->leFromM->text().toUInt();
    unsigned int v_filterFromS = ui->leFromS->text().toUInt();
    unsigned int v_filterToH = ui->leToH->text().toUInt();
    unsigned int v_filterToM = ui->leToM->text().toUInt();
    unsigned int v_filterToS = ui->leToS->text().toUInt();
    unsigned int v_fromFullTime = v_filterFromH*60*60 + v_filterFromM*60 + v_filterFromS;
    unsigned int v_toFullTime = v_filterToH*60*60 + v_filterToM*60 + v_filterToS;

    qDebug() << "v_fromFullTime=" << v_fromFullTime << "\t\tv_toFullTime" << v_toFullTime << endl;

    // Loop over all the entries of the branch in the tree
    for (unsigned int indexInRawData=0; indexInRawData<v_NumOfEntries; indexInRawData++)
    {
        // Read entry
        v_recDataBranch->GetEntry(indexInRawData);

        // check the filter
        if (!ui->groupBox->isChecked()) {
            // Filtering is off
            mImportedData.push_back(v_dataObject);
        } else {
            // Filtering is on
            unsigned int v_curFullTime = v_dataObject.tm_hour*60*60 + v_dataObject.tm_min*60 + v_dataObject.tm_sec;
            if (v_curFullTime > v_fromFullTime && v_curFullTime < v_toFullTime) {
                mImportedData.push_back(v_dataObject);
            }
        }
    }

    v_dataFile->Close();

    return 0; // ok
}

// 1 - fail
// 0 - ok
unsigned int cls_MainWindow::Export(QString p_filename)
{
    char v_filename[255];
    strcpy(v_filename, p_filename.toStdString().c_str());

    qDebug() << "Exporting into " << v_filename << endl;

    // Get the file
    TFile* v_dataFile = new TFile(v_filename, "RECREATE");
    if (v_dataFile->IsZombie()) {
        qDebug() << "Error opening file " << v_filename << endl;
        return 1; // FAIL
    }

    struct rec_packet recordedPack;
    TTree* dataTree = new TTree("dataTree", "dataTree");
    dataTree->Branch("recordedData", &recordedPack.second_count, "second_count/i:pc_counter_1/i:tm_sec/b:tm_min/b:tm_hour/b:tm_year/s:tm_yday/s");

    std::vector<struct rec_packet>::iterator iter;
    for (iter=mImportedData.begin(); iter != mImportedData.end(); ++iter) {
        recordedPack = *iter;
        dataTree->Fill();
    }

    v_dataFile->Write();
    v_dataFile->Close();
    return 0; // ok
}

//TODO check file open status
unsigned int cls_MainWindow::ExportAsText()
{
    QString p_filename = ui->lineEdit_2->text();

    char v_filename[255];
    strcpy(v_filename, p_filename.toStdString().c_str());

    qDebug() << "Exporting into " << v_filename << endl;

    std::ofstream myfile;
    myfile.open (v_filename);

    if (!myfile.is_open()) {
        qDebug() << "Failed to open " << v_filename << " for export." << endl;
        return 1; // FAIL
    }

    std::vector<struct rec_packet>::iterator iter;
    for (iter=mImportedData.begin(); iter != mImportedData.end(); ++iter)
    {
        myfile << std::setfill('0')
               << (uint32_t)iter->tm_year << " " << (uint32_t)iter->tm_yday << " "
               << std::setw(2) << (uint32_t)iter->tm_hour << " "
               << std::setw(2) << (uint32_t)iter->tm_min << " "
               << std::setw(2) << (uint32_t)iter->tm_sec << " "
               << iter->second_count << " " << iter->pc_counter_1 << endl;
    }

    myfile.close();

    return 0; // OK
}

unsigned int cls_MainWindow::ReadFilterFile(void)
{
    QFileDialog v_dial;
    v_dial.setFileMode(QFileDialog::ExistingFile);
    v_dial.setNameFilter(tr("Text files (*.txt)"));
    v_dial.setDirectory("/home/evovch/Documents/DEVELOPMENT/Ivanov/data");

    QStringList v_fileNames;
    if (v_dial.exec())
        v_fileNames = v_dial.selectedFiles();

    QString v_curLine = v_fileNames.at(0);
    char v_filename[255];
    strcpy(v_filename, v_curLine.toStdString().c_str());

    std::ifstream v_myfile (v_filename);
    //unsigned int fromH, fromM, fromS, toH, toM, toS;
    std::string line;
    //std::vector<std::string> parseOutput;
    std::string delims(" :-");
    if (v_myfile.is_open())
    {
        while (getline(v_myfile, line)) {
            //parseOutput = tokenize(line, delims);
            //cout << "Read: " << parseOutput[0] << ":" << parseOutput[1] << ":" << parseOutput[2] << " - "
            //     << parseOutput[3] << ":" << parseOutput[4] << ":" << parseOutput[5] << endl;
            ui->lstFilters->addItem(QString::fromStdString(line));
        }
    }

    v_myfile.close();

    return 0;
}

void cls_MainWindow::DrawAllDist(void)
{
    std::string line;
    std::string delims(" :-");
    std::vector<std::string> parseOutput;
    for (int i=0; i<ui->lstFilters->count(); i++) {

        QListWidgetItem* v_item = ui->lstFilters->item(i);
        line = v_item->text().toStdString();

        parseOutput = tokenize(line, delims);

        cout << "Parsed: " << parseOutput[0] << ":" << parseOutput[1] << ":" << parseOutput[2] << " - "
             << parseOutput[3] << ":" << parseOutput[4] << ":" << parseOutput[5] << endl;

        ui->leFromH->setText(QString::fromStdString(parseOutput[0]));
        ui->leFromM->setText(QString::fromStdString(parseOutput[1]));
        ui->leFromS->setText(QString::fromStdString(parseOutput[2]));
        ui->leToH->setText(QString::fromStdString(parseOutput[3]));
        ui->leToM->setText(QString::fromStdString(parseOutput[4]));
        ui->leToS->setText(QString::fromStdString(parseOutput[5]));

        this->Merge();
    }
}
