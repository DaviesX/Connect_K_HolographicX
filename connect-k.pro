TEMPLATE = app

TARGET = HolographicXAI

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function

CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt


#INCLUDEPATH += src

SOURCES += \
    src/iheuristic.cpp \
    src/istrategy.cpp \
    src/log.cpp \
    src/mainshell.cpp \
    src/move.cpp \
    src/state.cpp \
    src/stopwatch.cpp \
    src/strategydfs.cpp \
    src/strategyfactory.cpp \
    src/strategyrand.cpp \
    src/heurchessdeg.cpp \
    src/heurcostbenefit.cpp

HEADERS += \
    src/iheuristic.h \
    src/istrategy.h \
    src/log.h \
    src/move.h \
    src/state.h \
    src/stopwatch.h \
    src/strategydfs.h \
    src/strategyfactory.h \
    src/strategyrand.h \
    src/tst_state.h \
    src/tst_strategydfs.h \
    src/heurchessdeg.h \
    src/heurcostbenefit.h \
    src/tst_heurchessdeg.h \
    src/tst_heurcostbenefit.h
