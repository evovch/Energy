QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Root_files_merger
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    ../Analysis_module/GraphWidget.cpp \
    ../Analysis_module/HistoWidget.cpp \
    ../Analysis_module/Go4QtRoot/QRootApplication.cpp \
    ../Analysis_module/Go4QtRoot/QRootCanvas.cpp \
    ../Analysis_module/Go4QtRoot/QRootDialog.cpp \
    ../Analysis_module/Go4QtRoot/QRootWindow.cpp

HEADERS += \
    MainWindow.h \
    ../Analysis_module/data_structures.h \
    ../Analysis_module/GraphWidget.h \
    ../Analysis_module/HistoWidget.h \
    ../Analysis_module/Go4QtRoot/QRootApplication.h \
    ../Analysis_module/Go4QtRoot/QRootCanvas.h \
    ../Analysis_module/Go4QtRoot/QRootDialog.h \
    ../Analysis_module/Go4QtRoot/QRootWindow.h \
    ../Analysis_module/Go4QtRoot/TGo4LockGuard.h

FORMS += \
    cls_mainwindow.ui \
    ../Analysis_module/GraphWidget.ui \
    ../Analysis_module/HistoWidget.ui

QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Wpedantic

INCLUDEPATH += \
    ../Analysis_module \
    ../Analysis_module/Go4QtRoot \
    /home/evovch/soft/root_install/include

LIBS += -lX11 -lgsl -lgslcblas \
    -L/home/evovch/soft/root_install/lib \
    -lGui -lCore -lCling -lRIO -lNet -lHist -lGraf -lGraf3d \
    -lGpad -lTree -lRint -lPostscript -lMatrix \
    -lPhysics -lMathCore -lThread \
    -lGed -lTreePlayer \
    -pthread -lm -ldl -rdynamic
