QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Analysis_module
TEMPLATE = app

SOURCES += \
    main.cpp \
    Go4QtRoot/QRootCanvas.cpp \
    Go4QtRoot/QRootDialog.cpp \
    Go4QtRoot/QRootWindow.cpp \
    Go4QtRoot/QRootApplication.cpp \
    HistoWidget.cpp \
    GraphWidget.cpp \
    AnalysedData.cpp \
    Analyser.cpp \
    SSA_visu.cpp \
    MainWindow.cpp

HEADERS += \
    Go4QtRoot/QRootCanvas.h \
    Go4QtRoot/QRootDialog.h \
    Go4QtRoot/QRootWindow.h \
    Go4QtRoot/QRootApplication.h \
    Go4QtRoot/TGo4LockGuard.h \
    HistoWidget.h \
    AnalysedData.h \
    data_structures.h \
    Analyser.h \
    SSA_visu.h \
    GraphWidget.h \
    MainWindow.h

FORMS += \
    HistoWidget.ui \
    GraphWidget.ui \
    MainWindow.ui \
    SSA_visu.ui

QMAKE_CXXFLAGS += -std=c++11 -Wall

INCLUDEPATH += \
    Go4QtRoot/ \
    /home/evovch/soft/root_install/include

LIBS += -lX11 -lgsl -lgslcblas \
    -L/home/evovch/soft/root_install/lib \
    -lGui -lCore -lCling -lRIO -lNet -lHist -lGraf -lGraf3d \
    -lGpad -lTree -lRint -lPostscript -lMatrix \
    -lPhysics -lMathCore -lThread \
    -lGed -lTreePlayer \
    -pthread -lm -ldl -rdynamic
