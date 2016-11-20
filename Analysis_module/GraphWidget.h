#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>

#include <Rtypes.h>
#include <memory>

#include <gsl/gsl_vector.h>
#include "data_structures.h"

class TGraph;

namespace Ui {
class cls_GraphWidget;
}

enum enu_FillFromPacketType {
    etnSecStraight,
    etnPcStraight,
    etnSecDiff,
    etnPcDiff
};

class cls_GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit cls_GraphWidget(QWidget *parent = 0);
    cls_GraphWidget(char* p_name, QWidget *parent = 0);
    ~cls_GraphWidget();

    void FillFromData(std::vector<Double_t> p_data);
    void FillFromData(gsl_vector* p_data);
    void FillFromData(std::vector<struct rec_packet> p_data, enu_FillFromPacketType p_fillType = etnPcDiff);

    void SetDrawOptions(const char* p_opt) { mDrawOptions = QString(p_opt); }

public slots:
    void FillTest(void);
    void SetLogY(Int_t p_val = 1);
    void ToggleLogY(void);
    void UnZoom(void);


private:
    Ui::cls_GraphWidget *ui;

    std::unique_ptr<TGraph> mGraph;

    QString mDrawOptions;

    static UInt_t mGraphID;
};

#endif // GRAPHWIDGET_H
