#ifndef SSA_VISU_H
#define SSA_VISU_H

#include <QWidget>






class cls_Analyser;

namespace Ui {
class cls_SSA_visu;
}

class cls_SSA_visu : public QWidget
{
    Q_OBJECT

public:
    explicit cls_SSA_visu(QWidget *parent = 0);
    ~cls_SSA_visu();

    void LoadHistos(cls_Analyser* p_analyser);

private:
    Ui::cls_SSA_visu *ui;
};

#endif // SSA_VISU_H
