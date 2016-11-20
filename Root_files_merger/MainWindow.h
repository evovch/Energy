#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "data_structures.h"
#include <vector>

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
    void ChooseFile(void);
    unsigned int Merge(void);
    unsigned int ReadFilterFile(void);
    void DrawAllDist(void);
    unsigned int ExportAsText();

private:
    Ui::cls_MainWindow *ui;

    std::vector<struct rec_packet> mImportedData;

    unsigned int Append(QString p_filename);

    unsigned int Export(QString p_filename);

};

#endif // MAINWINDOW_H
