#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class cls_AnalysedData;
class cls_SSA_visu;

namespace Ui {
class cls_MainWindow;
}

class cls_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit cls_MainWindow(QWidget *parent = 0);
    ~cls_MainWindow();

public slots:
    void LoadFile(void);
    void Reload(void);
    void RunSSA(void);

private:
    Ui::cls_MainWindow *ui;

    std::unique_ptr<cls_AnalysedData> mDataObject;
};

#endif // MAINWINDOW_H
