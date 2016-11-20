#ifndef HISTOWIDGET_H
#define HISTOWIDGET_H

#include <QWidget>

#include <Rtypes.h>
#include <memory>

#include "data_structures.h"

class TH1D;

namespace Ui {
class cls_HistoWidget;
}

class cls_HistoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit cls_HistoWidget(QWidget *parent = 0);
    ~cls_HistoWidget();

    void FillFromData(std::vector<Double_t> p_data);
    void FillFromData(std::vector<struct rec_packet> p_data);

public slots:
    void FillTest(void);
    void SetLogY(Int_t p_val = 1);
    void ToggleLogY(void);
    void UnZoom(void);
    void RebinBy2(void);
    void ExportEPS(void);
    void PlusThickness(void);
    void MinusThickness(void);
    void Rename(void);
    void MoveYtitleLeft(void);

private:
    Ui::cls_HistoWidget *ui;

    std::unique_ptr<TH1D> mHisto;

    static UInt_t mHistoID;
};

#endif // HISTOWIDGET_H
