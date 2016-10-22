TEMPLATE = app
TARGET = HolographicXAI
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += src

SOURCES += \
    src/heursuccesslink.cpp \
    src/iheuristic.cpp \
    src/istrategy.cpp \
    src/log.cpp \
    src/mainshell.cpp \
    src/move.cpp \
    src/state.cpp \
    src/stopwatch.cpp \
    src/strategydfs.cpp \
    src/strategyfactory.cpp \
    src/strategyrand.cpp

HEADERS += \
    src/heursuccesslink.h \
    src/iheuristic.h \
    src/istrategy.h \
    src/log.h \
    src/move.h \
    src/state.h \
    src/stopwatch.h \
    src/strategydfs.h \
    src/strategyfactory.h \
    src/strategyrand.h \
    src/tst_heursuccesslink.h \
    src/tst_state.h \
    src/tst_strategydfs.h
