#include "MainWindow.h"
#include <TApplication.h>
#include "QRootApplication.h"

int main(int argc, char *argv[])
{
    TApplication app("AnalysisGUI", &argc, argv);
    QRootApplication myapp(argc, argv);

    cls_MainWindow w;
    w.show();

    return myapp.exec();
}
